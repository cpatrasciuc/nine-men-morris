// Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "graphics/post_effects.h"

#include "base/log.h"
#include "graphics/ogre_app.h"

#include "OGRE/OgreCompositorChain.h"
#include "OGRE/OgreCompositorManager.h"
#include "OGRE/OgreRenderWindow.h"
#include "OGRE/OgreViewport.h"

namespace graphics {
namespace {

const char kVignetteCompositorName[] = "Vignette";

}  // anonymous namespace

void SetVignettingEnabled(bool enable) {
  OgreApp& app = OgreApp::Instance();
  DCHECK_EQ(app.window()->getNumViewports(), 1);
  Ogre::Viewport* const viewport = app.window()->getViewport(0);
  Ogre::CompositorManager& comp_mgr = Ogre::CompositorManager::getSingleton();
  Ogre::CompositorChain* const chain = comp_mgr.getCompositorChain(viewport);
  if (enable) {
    if (!chain->getCompositor(kVignetteCompositorName)) {
      comp_mgr.addCompositor(viewport, kVignetteCompositorName);
    }
    comp_mgr.setCompositorEnabled(viewport, kVignetteCompositorName, enable);
  } else if (chain->getCompositor(kVignetteCompositorName)) {
    comp_mgr.removeCompositor(viewport, kVignetteCompositorName);
  }
}

}  // namespace graphics
