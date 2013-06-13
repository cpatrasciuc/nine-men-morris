// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/board_renderer.h"

#include <algorithm>
#include <deque>
#include <string>
#include <vector>

#include "base/string_util.h"
#include "base/supports_listener.h"

#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"

#include "graphics/board_utils.h"
#include "graphics/ogre_app.h"

#include "OGRE/OgreCamera.h"
#include "OGRE/OgreColourValue.h"
#include "OGRE/OgreCommon.h"
#include "OGRE/OgreEntity.h"
#include "OGRE/OgreHardwarePixelBuffer.h"
#include "OGRE/OgreLight.h"
#include "OGRE/OgreMaterial.h"
#include "OGRE/OgreMaterialManager.h"
#include "OGRE/OgreMeshManager.h"
#include "OGRE/OgreMovableObject.h"
#include "OGRE/OgrePlane.h"
#include "OGRE/OgreRay.h"
#include "OGRE/OgreRenderTexture.h"
#include "OGRE/OgreResourceGroupManager.h"
#include "OGRE/OgreRoot.h"
#include "OGRE/OgreSceneManager.h"
#include "OGRE/OgreSceneNode.h"
#include "OGRE/OgreSceneQuery.h"
#include "OGRE/OgreTextureManager.h"
#include "OGRE/OgreVector3.h"

#include "OIS/OISMouse.h"

namespace graphics {
namespace {

const char kBoardEntityName[] = "BoardEntity";
const char kBoardMaterialName[] = "BoardMaterial";
const char kBoardPlaneName[] = "BoardPlane";
const char kBoardTextureName[] = "BoardTexture";
const char kLocationMaterialName[] = "LocationMaterial";
const char kAllPiecesNodeName[] = "AllPieces";
const char kMainLightName[] = "MainLight";
const char kRttCameraName[] = "RttCamera";

const char kWhitePieceMaterialName[] = "WhitePieceMaterial";
const char kBlackPieceMaterialName[] = "BlackPieceMaterial";

const int kBoardTextureSize = 3;

}  // anonymous namespace

SelectionListener::SelectionListener() {}

SelectionListener::~SelectionListener() {}

BoardRenderer::BoardRenderer(OgreApp* app, const game::Game& game_model)
    : app_(app),
      game_(game_model),
      temp_selected_location_(NULL),
      selected_location_(NULL),
      selection_type_(NONE) {}

BoardRenderer::~BoardRenderer() {
  Ogre::MaterialManager::getSingleton().remove(kBoardMaterialName);
  Ogre::MaterialManager::getSingleton().remove(kLocationMaterialName);
  Ogre::MaterialManager::getSingleton().remove(kWhitePieceMaterialName);
  Ogre::MaterialManager::getSingleton().remove(kBlackPieceMaterialName);
  Ogre::MeshManager::getSingleton().remove(kBoardPlaneName);
  Ogre::TextureManager::getSingleton().remove(kBoardTextureName);
}

void BoardRenderer::Initialize() {
  GenerateBoardTexture();

  Ogre::SceneManager* const scene_manager = app_->scene_manager();
  scene_manager->setAmbientLight(Ogre::ColourValue::Black);
  scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

  const int multiplier = 10;
  const int board_size = kBoardTextureSize * multiplier;
  Ogre::Camera* const camera = app_->camera();
  camera->setPosition(board_size, board_size, board_size);
  camera->lookAt(Ogre::Vector3::ZERO);

  Ogre::Light* const main_light = scene_manager->createLight(kMainLightName);
  main_light->setType(Ogre::Light::LT_DIRECTIONAL);
  main_light->setDiffuseColour(Ogre::ColourValue::White);
  main_light->setSpecularColour(Ogre::ColourValue::White);
  main_light->setCastShadows(true);
  main_light->setDirection(board_size, -board_size, -board_size);

  const Ogre::Plane board_plane(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane(kBoardPlaneName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      board_plane, board_size, board_size, multiplier, multiplier,
      true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
  Ogre::Entity* const board_entity =
      scene_manager->createEntity(kBoardEntityName, kBoardPlaneName);
  Ogre::SceneNode* const root = scene_manager->getRootSceneNode();
  root->createChildSceneNode()->attachObject(board_entity);

  Ogre::MaterialPtr board_material =
      Ogre::MaterialManager::getSingleton().create(kBoardMaterialName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  board_material->getTechnique(0)->getPass(0)
      ->createTextureUnitState(kBoardTextureName);
  board_entity->setMaterial(board_material);
  board_entity->setCastShadows(false);

  const std::vector<game::BoardLocation>& locations = game_.board().locations();
  Ogre::MaterialPtr sphere_material =
      Ogre::MaterialManager::getSingleton().create(kLocationMaterialName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  sphere_material->setDiffuse(1.0, 0, 0, 0.5);
  sphere_material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  sphere_material->setDepthWriteEnabled(false);

  for (size_t i = 0; i < locations.size(); ++i) {
    const std::string name = "Location" + base::ToString(i);
    Ogre::Entity* sphere_entity =
        scene_manager->createEntity(name, Ogre::SceneManager::PT_SPHERE);
    Ogre::SceneNode* sphere_node = root->createChildSceneNode();
    sphere_node->attachObject(sphere_entity);
    Ogre::Vector3 pos = BoardLocationTo3DCoord(locations[i], game_.board());
    pos.z = pos.y;
    pos.y = 0;
    sphere_node->setPosition(pos * multiplier);
    sphere_entity->setMaterial(sphere_material);
    sphere_entity->setCastShadows(false);
    const double scale = 0.005f / game_.board().size() * multiplier * 5;
    sphere_node->setScale(scale, 0.001, scale);
    sphere_entity->setVisible(false);
    loc_map_.insert(std::make_pair(sphere_entity, locations[i]));
  }

  InitializePieces();
}

void BoardRenderer::SetSelectionType(const SelectionType& selection_type) {
  selection_type_ = selection_type;
  if (selection_type_ == NONE) {
    ClearSelection();
  }
  // TODO(board_renderer): For REMOVABLE_* determine the selectable items.
  // TODO(board_renderer): Trigger a ray cast; don't wait for mouse movement.
}

bool BoardRenderer::mouseMoved(const OIS::MouseEvent& event) {
  if (selection_type_ == NONE) {
    return true;
  }
  if (temp_selected_location_) {
    temp_selected_location_->setVisible(false);
    temp_selected_location_ = NULL;
  }
  Ogre::SceneManager* const scene_manager = app_->scene_manager();
  Ogre::Camera* const camera = app_->camera();
  const OIS::MouseState& mouse_state = app_->mouse().getMouseState();
  const Ogre::Ray ray = camera->getCameraToViewportRay(
      mouse_state.X.abs / double(mouse_state.width),
      mouse_state.Y.abs / double(mouse_state.height));
  Ogre::RaySceneQuery* ray_scene_query = scene_manager->createRayQuery(ray);
  ray_scene_query->setSortByDistance(true);
  const Ogre::RaySceneQueryResult query_result = ray_scene_query->execute();
  for (size_t i = 0; i < query_result.size(); ++i) {
    if (!query_result[i].movable) {
      continue;
    }
    // TODO(selection): Replace this test with query mask bits
    if (query_result[i].movable->getName().substr(0, 8) != "Location") {
      continue;
    }
    temp_selected_location_ = query_result[i].movable;
    temp_selected_location_->setVisible(true);
    break;
  }
  scene_manager->destroyQuery(ray_scene_query);
  return true;
}

bool BoardRenderer::mousePressed(const OIS::MouseEvent& event,
                                 OIS::MouseButtonID id) {
  return true;
}

bool BoardRenderer::mouseReleased(const OIS::MouseEvent& event,
                                  OIS::MouseButtonID id) {
  ClearSelection();
  if (temp_selected_location_) {
    selected_location_ = temp_selected_location_;
    std::map<Ogre::MovableObject*, game::BoardLocation>::iterator it =
        loc_map_.find(selected_location_);
    FireOnLocationSelected(it->second);
  }
  return true;
}

void BoardRenderer::GenerateBoardTexture() {
  Ogre::TexturePtr rtt_tex = Ogre::TextureManager::getSingleton().createManual(
      kBoardTextureName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      Ogre::TEX_TYPE_2D,
      1024, 1024, 0,
      Ogre::PF_R8G8B8,
      Ogre::TU_RENDERTARGET);

  Ogre::SceneManager* const scene_manager =
      app_->ogre_root()->createSceneManager(Ogre::ST_GENERIC);
  Ogre::Camera* const camera = scene_manager->createCamera(kRttCameraName);
  camera->setPosition(Ogre::Vector3(0, 0, 1000));
  camera->lookAt(Ogre::Vector3::ZERO);
  camera->setAspectRatio(1.0f);
  camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
  camera->setOrthoWindow(kBoardTextureSize, kBoardTextureSize);

  Ogre::RenderTexture* render_texture = rtt_tex->getBuffer()->getRenderTarget();
  render_texture->addViewport(camera);
  render_texture->getViewport(0)->setClearEveryFrame(true);
  render_texture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
  render_texture->getViewport(0)->setOverlaysEnabled(false);

  const game::Board& board = game_.board();
  const std::vector<game::BoardLocation>& locations = board.locations();
  Ogre::SceneNode* const scene_contents =
      scene_manager->getRootSceneNode()->createChildSceneNode();
  const double scale = board.size() / 2;
  for (size_t i = 0; i < locations.size(); ++i) {
    const std::string name = "Sphere" + base::ToString(i);
    Ogre::Entity* sphere_entity =
        scene_manager->createEntity(name, Ogre::SceneManager::PT_SPHERE);
    Ogre::SceneNode* sphere_node = scene_contents->createChildSceneNode();
    sphere_node->attachObject(sphere_entity);
    sphere_node->setPosition(BoardLocationTo3DCoord(locations[i], board));
    const double radius = 0.005f / scale;
    sphere_node->setScale(radius, radius, radius);

    const Ogre::Vector3 v1 = BoardLocationTo3DCoord(locations[i], board);
    std::vector<game::BoardLocation> adjacent;
    board.GetAdjacentLocations(locations[i], &adjacent);
    for (size_t j = 0; j < adjacent.size(); ++j) {
      const std::string name = "Line" + base::ToString(i) + base::ToString(j);
      Ogre::Entity* line_entity =
          scene_manager->createEntity(name, Ogre::SceneManager::PT_PLANE);
      Ogre::SceneNode* line_node = scene_contents->createChildSceneNode();
      line_node->attachObject(line_entity);
      const Ogre::Vector3 v2 = BoardLocationTo3DCoord(adjacent[j], board);
      line_node->setPosition(v1.midPoint(v2));
      const double width = 0.0005 / scale;
      const double depth = 0.0001;
      if (locations[i].line() == adjacent[j].line()) {
        const double length =
            0.005 * (locations[i].column() - adjacent[j].column()) / scale;
        line_node->setScale(width, length, depth);
      } else {
        const double length =
            0.005f * (locations[i].line() - adjacent[j].line()) / scale;
        line_node->setScale(length, width, depth);
      }
    }
  }
  render_texture->update();
#ifdef DEBUG_MODE
  render_texture->writeContentsToFile("board_texture.png");
#endif
  app_->ogre_root()->destroySceneManager(scene_manager);
}

void BoardRenderer::InitializePieces() {
  Ogre::MaterialPtr white_material =
      Ogre::MaterialManager::getSingleton().create(kWhitePieceMaterialName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  white_material->setDiffuse(0.8, 0.8, 0.8, 1);
  white_material->setSpecular(0.8, 0.8, 0.8, 1);
  Ogre::MaterialPtr black_material =
      Ogre::MaterialManager::getSingleton().create(kBlackPieceMaterialName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  black_material->setDiffuse(1, 0.2, 0.2, 1);

  const std::string mesh_name("piece.mesh");
  Ogre::SceneManager* const scene_mgr = app_->scene_manager();
  Ogre::SceneNode* const all_pieces =
      scene_mgr->getRootSceneNode()->createChildSceneNode(kAllPiecesNodeName);
  const int piece_count =
      game_.GetInitialPieceCountByGameType(game_.options().game_type());

  for (int i = 0; i < piece_count; ++i) {
    const std::string index_str(base::ToString(i));
    Ogre::Entity* entity = scene_mgr->createEntity(
        "WhitePiece" + index_str, mesh_name);
    entity->setMaterial(white_material);
    entity->setVisible(false);
    Ogre::SceneNode* piece_node = all_pieces->createChildSceneNode();
    piece_node->attachObject(entity);

    entity = scene_mgr->createEntity("BlackPiece" + index_str, mesh_name);
    entity->setMaterial(black_material);
    entity->setVisible(false);
    piece_node = all_pieces->createChildSceneNode();
    piece_node->attachObject(entity);
  }
}

void BoardRenderer::FireOnLocationSelected(const game::BoardLocation& loc) {
  typedef base::SupportsListener<SelectionListener>::ListenerIter ListenerIter;
  for (ListenerIter it = listeners().begin(); it != listeners().end(); ++it) {
    (*it)->OnLocationSelected(loc);
  }
}

void BoardRenderer::FireOnSelectionCleared() {
  typedef base::SupportsListener<SelectionListener>::ListenerIter ListenerIter;
  for (ListenerIter it = listeners().begin(); it != listeners().end(); ++it) {
    (*it)->OnSelectionCleared();
  }
}

void BoardRenderer::ClearSelection() {
  if (selected_location_) {
    selected_location_->setVisible(false);
    selected_location_ = NULL;
    FireOnSelectionCleared();
  }
}

}  // namespace graphics
