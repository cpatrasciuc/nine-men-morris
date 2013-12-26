// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/board_view.h"

#include <algorithm>
#include <string>
#include <vector>

#include "base/log.h"
#include "base/string_util.h"
#include "base/supports_listener.h"

#include "game/board.h"
#include "game/board_location.h"
#include "game/game.h"
#include "game/piece_color.h"
#include "game/player_action.h"

#include "graphics/ogre_app.h"
#include "graphics/selection_listener.h"

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
#include "OGRE/OgreParticleEmitter.h"
#include "OGRE/OgreParticleSystem.h"
#include "OGRE/OgrePlane.h"
#include "OGRE/OgreRay.h"
#include "OGRE/OgreRenderQueue.h"
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
const char kBoardViewRootNodeName[] = "BoardViewRoot";
const char kLocationMaterialName[] = "LocationMaterial";
const char kAllPiecesNodeName[] = "AllPieces";
const char kMainLightName[] = "MainLight";
const char kRttCameraName[] = "RttCamera";

const char kWhitePieceMaterialName[] = "WhitePieceMaterial";
const char kSelectedWhitePieceMaterialName[] = "SelectedWhitePieceMaterial";
const char kBlackPieceMaterialName[] = "BlackPieceMaterial";
const char kSelectedBlackPieceMaterialName[] = "SelectedBlackPieceMaterial";

const int kBoardTextureSize = 3;
const int kBoardSize = kBoardTextureSize * 10;

Ogre::Vector3 BoardLocationTo3DCoord(const game::BoardLocation& location,
                                     const game::Board& board) {
  const int delta = board.size() / 2;
  const double scale = board.size() / 2;
  return Ogre::Vector3((location.line() - delta) / scale,
                       (location.column() - delta) / scale,
                       0.0f);
}

}  // anonymous namespace

BoardView::BoardView(const game::Game& game_model)
    : app_(&OgreApp::Instance()),
      game_(game_model),
      white_place_index_(0),
      black_place_index_(0),
      white_pieces_(NULL),
      black_pieces_(NULL),
      temp_selected_location_(NULL),
      selected_location_(NULL),
      selection_type_(NONE) {}

BoardView::~BoardView() {
  Ogre::MaterialManager::getSingleton().remove(kBoardMaterialName);
  Ogre::MaterialManager::getSingleton().remove(kLocationMaterialName);
  Ogre::MaterialManager::getSingleton().remove(kWhitePieceMaterialName);
  Ogre::MaterialManager::getSingleton().remove(kBlackPieceMaterialName);
  Ogre::MeshManager::getSingleton().remove(kBoardPlaneName);
  Ogre::TextureManager::getSingleton().remove(kBoardTextureName);
  app_->scene_manager()->clearScene();
}

void BoardView::Initialize() {
  GenerateBoardTexture();

  Ogre::SceneManager* const scene_manager = app_->scene_manager();
  scene_manager->setAmbientLight(Ogre::ColourValue::Black);
  scene_manager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

  const int multiplier = kBoardSize / kBoardTextureSize;
  Ogre::Camera* const camera = app_->camera();
  camera->setPosition(kBoardSize, kBoardSize, kBoardSize);
  camera->lookAt(Ogre::Vector3::ZERO);

  Ogre::MovableObject::setDefaultQueryFlags(0);

  Ogre::Light* const main_light = scene_manager->createLight(kMainLightName);
  main_light->setType(Ogre::Light::LT_DIRECTIONAL);
  main_light->setDiffuseColour(Ogre::ColourValue::White);
  main_light->setSpecularColour(Ogre::ColourValue::White);
  main_light->setCastShadows(true);
  main_light->setDirection(kBoardSize, -kBoardSize, -kBoardSize);

  const Ogre::Plane board_plane(Ogre::Vector3::UNIT_Y, 0);
  Ogre::MeshManager::getSingleton().createPlane(kBoardPlaneName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      board_plane, kBoardSize, kBoardSize, multiplier, multiplier,
      true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
  Ogre::Entity* const board_entity =
      scene_manager->createEntity(kBoardEntityName, kBoardPlaneName);
  Ogre::SceneNode* root = scene_manager->getRootSceneNode();
  root = root->createChildSceneNode(kBoardViewRootNodeName);
  root->createChildSceneNode()->attachObject(board_entity);

  board_entity->setMaterialName(kBoardMaterialName);
  board_entity->setCastShadows(false);

  const std::vector<game::BoardLocation>& locations = game_.board().locations();
  Ogre::MaterialPtr sphere_material =
      Ogre::MaterialManager::getSingleton().create(kLocationMaterialName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  sphere_material->setDiffuse(1.0, 0, 0, 0.5);
  sphere_material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  sphere_material->setDepthWriteEnabled(false);

  Ogre::SceneNode* locations_node = root->createChildSceneNode("Locations");
  for (size_t i = 0; i < locations.size(); ++i) {
    const std::string name = "Location" + base::ToString(i);
    Ogre::Entity* sphere_entity =
        scene_manager->createEntity(name, Ogre::SceneManager::PT_SPHERE);
    Ogre::SceneNode* sphere_node = locations_node->createChildSceneNode();
    sphere_node->attachObject(sphere_entity);
    Ogre::Vector3 pos = BoardLocationTo3DCoord(locations[i], game_.board());
    pos.z = pos.y;
    pos.y = 0;
    sphere_node->setPosition(pos * multiplier);
    sphere_entity->setMaterial(sphere_material);
    sphere_entity->setCastShadows(false);
    sphere_entity->setQueryFlags(EMPTY_LOCATION);
    sphere_entity->setRenderQueueGroup(Ogre::RENDER_QUEUE_9);
    const double scale = 0.005f / game_.board().size() * multiplier * 5;
    sphere_node->setScale(scale, 0.001, scale);
    sphere_entity->setVisible(false);
    locations_.insert(std::make_pair(sphere_entity, locations[i]));
    reverse_loc_map_.insert(std::make_pair(locations[i], sphere_entity));
    positions_.insert(std::make_pair(locations[i],
                                     &sphere_node->getPosition()));
  }

  InitializePieces(root);

  // TODO(board_view): Add a test for BoardView on a loaded game
  bool old_animations_enabled = animation_controller_.animations_enabled();
  animation_controller_.set_animations_enabled(false);
  std::vector<game::PlayerAction> actions;
  game_.DumpActionList(&actions);
  for (size_t i = 0; i < actions.size(); ++i) {
    OnPlayerAction(actions[i]);
  }
  animation_controller_.set_animations_enabled(old_animations_enabled);

  InitializeMillEffect(root);
}

void BoardView::SetSelectionType(unsigned int selection_type) {
  selection_type_ = selection_type;
  if (selection_type & REMOVABLE_BLACK_PIECE) {
    UpdateRemovablePieces(game::BLACK_COLOR);
  }
  if (selection_type & REMOVABLE_WHITE_PIECE) {
    UpdateRemovablePieces(game::WHITE_COLOR);
  }
  UpdateSelection(app_->mouse().getMouseState());
}

void BoardView::SetCustomSelectableLocations(
    const std::vector<game::BoardLocation>& selectable) {
  const std::vector<game::BoardLocation>& locations = game_.board().locations();
  for (size_t i = 0; i < locations.size(); ++i) {
    DCHECK(reverse_loc_map_.count(locations[i]));
    Ogre::MovableObject* const entity = reverse_loc_map_[locations[i]];
    if (std::find(selectable.begin(), selectable.end(), locations[i]) !=
        selectable.end()) {
      entity->addQueryFlags(CUSTOM);
    } else {
      entity->removeQueryFlags(CUSTOM);
    }
  }
}

bool BoardView::mouseMoved(const OIS::MouseEvent& event) {
  UpdateSelection(event.state);
  return true;
}

bool BoardView::mousePressed(const OIS::MouseEvent& event,
                                 OIS::MouseButtonID id) {
  return true;
}

bool BoardView::mouseReleased(const OIS::MouseEvent& event,
                                  OIS::MouseButtonID id) {
  if (id != OIS::MB_Left) {
    return true;
  }
  ClearSelection();
  if (temp_selected_location_) {
    selected_location_ = temp_selected_location_;
    std::map<Ogre::MovableObject*, game::BoardLocation>::iterator it =
        locations_.find(selected_location_);
    DCHECK(it != locations_.end());
    const game::PieceColor color = game_.board().GetPieceAt(it->second);
    if (color != game::NO_COLOR) {
      Ogre::Entity* const entity = static_cast<Ogre::Entity*>(
          GetPieceByLocation(it->second)->getAttachedObject(0));
      entity->setMaterialName(color == game::WHITE_COLOR ?
          kSelectedWhitePieceMaterialName : kSelectedBlackPieceMaterialName);
    }
    FireOnLocationSelected(it->second);
  }
  return true;
}

void BoardView::GenerateBoardTexture() {
  // TODO(board): Blur a bit the blending texture.
  Ogre::TexturePtr rtt_tex = Ogre::TextureManager::getSingleton().createManual(
      kBoardTextureName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
      Ogre::TEX_TYPE_2D,
      1024, 1024, 0,
      Ogre::PF_R8G8B8A8,
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
  render_texture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::ZERO);
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

void BoardView::InitializePieces(Ogre::SceneNode* board_view_root) {
  // TODO(board_view): Improve material setup.
  Ogre::MaterialPtr white_material =
      Ogre::MaterialManager::getSingleton().create(
          kSelectedWhitePieceMaterialName,
          Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  white_material->setDiffuse(0.4, 0.4, 0.4, 0.5);
  white_material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  white_material->setDepthWriteEnabled(false);

  white_material = Ogre::MaterialManager::getSingleton().create(
      kWhitePieceMaterialName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  white_material->setDiffuse(0.4, 0.4, 0.4, 1);

  Ogre::MaterialPtr black_material =
      Ogre::MaterialManager::getSingleton().create(
          kSelectedBlackPieceMaterialName,
          Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  black_material->setDiffuse(0.8, 0.2, 0.2, 0.5);
  black_material->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
  black_material->setDepthWriteEnabled(false);

  black_material = Ogre::MaterialManager::getSingleton().create(
      kBlackPieceMaterialName,
      Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  black_material->setDiffuse(0.8, 0.2, 0.2, 1);

  const std::string mesh_name("piece.mesh");
  Ogre::SceneManager* const scene_mgr = app_->scene_manager();
  Ogre::SceneNode* const all_pieces =
      board_view_root->createChildSceneNode(kAllPiecesNodeName);
  white_pieces_ = all_pieces->createChildSceneNode("AllWhitePieces");
  black_pieces_ = all_pieces->createChildSceneNode("AllBlackPieces");
  const int piece_count =
      game_.GetInitialPieceCountByGameType(game_.options().game_type());

  const int pieces_in_a_row = game_.board().size() * 2 - 1;
  double scale = (kBoardSize / pieces_in_a_row) / 2.3;
  double white_diameter = 0.0;
  double black_diameter = 0.0;

  for (int i = 0; i < piece_count; ++i) {
    const std::string index_str(base::ToString(i));
    Ogre::Entity* entity = scene_mgr->createEntity(
        "WhitePiece" + index_str, mesh_name);
    entity->setMaterial(white_material);
    entity->setRenderQueueGroup(Ogre::RENDER_QUEUE_8);
    Ogre::SceneNode* piece_node = white_pieces_->createChildSceneNode();
    piece_node->attachObject(entity);
    if (i == 0) {
      scale /= entity->getBoundingRadius();
    }
    piece_node->scale(scale, scale, scale);
    white_diameter = entity->getBoundingRadius() * scale;

    entity = scene_mgr->createEntity("BlackPiece" + index_str, mesh_name);
    entity->setMaterial(black_material);
    entity->setRenderQueueGroup(Ogre::RENDER_QUEUE_8);
    piece_node = black_pieces_->createChildSceneNode();
    piece_node->attachObject(entity);
    piece_node->scale(scale, scale, scale);
    black_diameter = entity->getBoundingRadius() * scale;
  }

  DCHECK_GT(white_diameter, 0.0);
  DCHECK_GT(black_diameter, 0.0);

  for (int i = 0; i < piece_count; ++i) {
    // Compute the position index this way, instead of using |i| directly,
    // because we want the last played piece to be the one closest to the
    // corner of the board.
    const int position_index = piece_count - i - 1;
    double x = kBoardSize / 2.0 + 1.5 * white_diameter;
    double z = kBoardSize / 2.0 - (2.1 * position_index + 1) * white_diameter;
    GetPieceByColorAndIndex(game::WHITE_COLOR, i)->setPosition(x, 0, z);

    x *= black_diameter / white_diameter;
    z *= black_diameter / white_diameter;
    GetPieceByColorAndIndex(game::BLACK_COLOR, i)->setPosition(-x, 0, -z);
  }
}

void BoardView::InitializeMillEffect(Ogre::SceneNode* board_view_root) {
  Ogre::SceneManager* const scene_manager = app_->scene_manager();
  Ogre::ParticleSystem* const particle_system =
      scene_manager->createParticleSystem("MillHalo", "MillHalo");
  board_view_root->attachObject(particle_system);

  const std::vector<game::BoardLocation>& locations = game_.board().locations();

  Ogre::MovableObject* selector = GetLocationSelector(locations[0]);
  Ogre::SceneNode* node = selector->getParentSceneNode();
  const double radius = selector->getBoundingRadius() * node->getScale().x;

  particle_system->setRenderQueueGroup(Ogre::RENDER_QUEUE_8);
  particle_system->setDefaultHeight(1.5 * radius);
  particle_system->setDefaultWidth(1.5 * radius);

  for (size_t i = 0; i < locations.size(); ++i) {
    Ogre::ParticleEmitter* emitter = particle_system->addEmitter("Point");
    emitter->setPosition(Get3DPosition(locations[i]));
    emitter->setColour(Ogre::ColourValue(1, 1, 0));
    emitter->setDirection(Ogre::Vector3::UNIT_Y);
    emitter->setEmissionRate(15);
    emitter->setParticleVelocity(0.25);
    emitter->setTimeToLive(1);
    emitter->setEnabled(game_.board().IsPartOfMill(locations[i]));
    emitters_.insert(std::make_pair(locations[i], emitter));
  }

  particle_system->fastForward(15);
}

void BoardView::FireOnLocationSelected(const game::BoardLocation& loc) {
  typedef base::SupportsListener<SelectionListener>::ListenerList ListenerList;
  typedef base::SupportsListener<SelectionListener>::ListenerIter ListenerIter;
  const ListenerList list(listeners());
  for (ListenerIter it = list.begin(); it != list.end(); ++it) {
    (*it)->OnLocationSelected(loc);
  }
}

void BoardView::ClearSelection() {
  if (selected_location_) {
    std::map<Ogre::MovableObject*, game::BoardLocation>::iterator it =
        locations_.find(selected_location_);
    DCHECK(it != locations_.end());
    const game::PieceColor color = game_.board().GetPieceAt(it->second);
    if (color != game::NO_COLOR) {
      Ogre::Entity* const entity = static_cast<Ogre::Entity*>(
          GetPieceByLocation(it->second)->getAttachedObject(0));
      entity->setMaterialName(color == game::WHITE_COLOR ?
          kWhitePieceMaterialName : kBlackPieceMaterialName);
    }
    selected_location_->setVisible(false);
    selected_location_ = NULL;
  }
}

void BoardView::UpdateRemovablePieces(game::PieceColor color) {
  DCHECK(color != game::NO_COLOR);
  bool can_remove_from_mill = true;
  const std::vector<game::BoardLocation>& locations = game_.board().locations();

  for (size_t i = 0; i < locations.size(); ++i) {
    if (game_.board().GetPieceAt(locations[i]) == color) {
      if (!game_.board().IsPartOfMill(locations[i])) {
        can_remove_from_mill = false;
        break;
      }
    }
  }

  const unsigned int removable_piece =
      color == game::WHITE_COLOR ?
      (REMOVABLE_WHITE_PIECE | ANY_WHITE_PIECE) :
      (REMOVABLE_BLACK_PIECE | ANY_BLACK_PIECE);
  const unsigned int unremovable_piece =
      color == game::WHITE_COLOR ? ANY_WHITE_PIECE : ANY_BLACK_PIECE;

  for (size_t i = 0; i < locations.size(); ++i) {
    if (game_.board().GetPieceAt(locations[i]) == color) {
      Ogre::MovableObject* location = reverse_loc_map_[locations[i]];
      if (game_.board().IsPartOfMill(locations[i]) && !can_remove_from_mill) {
        location->setQueryFlags(unremovable_piece);
      } else {
        location->setQueryFlags(removable_piece);
      }
    }
  }
}

void BoardView::OnPlayerAction(const game::PlayerAction& action) {
  const game::PieceColor player = action.player_color();
  Ogre::MovableObject* location = NULL;

  switch (action.type()) {
    case game::PlayerAction::PLACE_PIECE:
      AddPiece(action.destination(), player);
      location = reverse_loc_map_[action.destination()];
      location->setQueryFlags(
          player == game::WHITE_COLOR ? ANY_WHITE_PIECE : ANY_BLACK_PIECE);
      break;

    case game::PlayerAction::REMOVE_PIECE:
      RemovePiece(action.source(), player);
      location = reverse_loc_map_[action.source()];
      location->setQueryFlags(EMPTY_LOCATION);
      break;

    case game::PlayerAction::MOVE_PIECE:
      MovePiece(action.source(), action.destination(), player);
      location = reverse_loc_map_[action.source()];
      location->setQueryFlags(EMPTY_LOCATION);
      location = reverse_loc_map_[action.destination()];
      location->setQueryFlags(
          player == game::WHITE_COLOR ? ANY_WHITE_PIECE : ANY_BLACK_PIECE);
      break;
  }
}

void BoardView::OnMillEvent(const game::BoardLocation& location, bool mill) {
  DCHECK(emitters_.count(location));
  emitters_[location]->setEnabled(mill);
}

Ogre::SceneNode* BoardView::GetPieceByColorAndIndex(game::PieceColor color,
                                                    int index) {
  const Ogre::SceneNode* const parent_node =
      color == game::WHITE_COLOR ? white_pieces_ : black_pieces_;
  return static_cast<Ogre::SceneNode*>(parent_node->getChild(index));
}

Ogre::SceneNode* BoardView::GetPieceByLocation(const game::BoardLocation& loc) {
  std::map<game::BoardLocation, int>::iterator it = white_index_map_.find(loc);
  if (it != white_index_map_.end()) {
    DCHECK(!black_index_map_.count(loc));
    return GetPieceByColorAndIndex(game::WHITE_COLOR, it->second);
  }
  it = black_index_map_.find(loc);
  DCHECK(it != black_index_map_.end());
  return GetPieceByColorAndIndex(game::BLACK_COLOR, it->second);
}

Ogre::MovableObject* BoardView::GetLocationSelector(
    const game::BoardLocation& loc) {
  DCHECK(reverse_loc_map_.count(loc));
  return reverse_loc_map_[loc];
}

const Ogre::Vector3& BoardView::Get3DPosition(
    const game::BoardLocation& location) const {
  const std::map<game::BoardLocation, const Ogre::Vector3*>::const_iterator it =
      positions_.find(location);
  DCHECK(it != positions_.end());
  return *(it->second);
}

BoardView::IndexMap* BoardView::GetIndexMapByColor(game::PieceColor color) {
  DCHECK(color != game::NO_COLOR);
  return color == game::WHITE_COLOR ? &white_index_map_ : &black_index_map_;
}

void BoardView::MovePiece(const game::BoardLocation& from,
                          const game::BoardLocation& to,
                          game::PieceColor color) {
  Ogre::SceneNode* const piece_node = GetPieceByLocation(from);
  animation_controller_.AddMoveAnimation(piece_node, Get3DPosition(to));
  IndexMap& index_map = *GetIndexMapByColor(color);
  index_map[to] = index_map[from];
  index_map.erase(from);
}

void BoardView::AddPiece(const game::BoardLocation& to,
                         game::PieceColor color) {
  int* const index = color == game::WHITE_COLOR ?
      &white_place_index_ : &black_place_index_;
  Ogre::SceneNode* const piece_node = GetPieceByColorAndIndex(color, *index);
  animation_controller_.AddMoveAnimation(piece_node, Get3DPosition(to));
  IndexMap& index_map = *GetIndexMapByColor(color);
  index_map[to] = *index;
  ++(*index);
}

void BoardView::RemovePiece(const game::BoardLocation& from,
                            game::PieceColor color) {
  Ogre::SceneNode* const piece_node = GetPieceByLocation(from);
  animation_controller_.AddRemoveAnimation(piece_node);
  IndexMap* index_map = GetIndexMapByColor(game::GetOpponent(color));
  index_map->erase(from);
}

void BoardView::UpdateSelection(const OIS::MouseState& mouse_state) {
  if (temp_selected_location_) {
    temp_selected_location_->setVisible(false);
    temp_selected_location_ = NULL;
  }
  if (selection_type_ == NONE) {
    return;
  }
  Ogre::SceneManager* const scene_manager = app_->scene_manager();
  Ogre::Camera* const camera = app_->camera();
  const Ogre::Ray ray = camera->getCameraToViewportRay(
      mouse_state.X.abs / double(mouse_state.width),
      mouse_state.Y.abs / double(mouse_state.height));
  Ogre::RaySceneQuery* ray_scene_query = scene_manager->createRayQuery(ray);
  ray_scene_query->setSortByDistance(true);
  ray_scene_query->setQueryMask(selection_type_);
  const Ogre::RaySceneQueryResult query_result = ray_scene_query->execute();
  for (size_t i = 0; i < query_result.size(); ++i) {
    if (!query_result[i].movable) {
      continue;
    }
    temp_selected_location_ = query_result[i].movable;
    temp_selected_location_->setVisible(true);
    break;
  }
  scene_manager->destroyQuery(ray_scene_query);
}

}  // namespace graphics
