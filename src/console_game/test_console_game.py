#!/usr/bin/python
# Copyright (c) 2013 Cristian Patrasciuc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import os
import subprocess
import sys
import tempfile
import unittest

class ConsoleGameFunctionalTests(unittest.TestCase):
  
  def getExeName(self):
    folder = os.path.abspath(os.path.dirname(__file__))
    return os.path.join(folder, "console_game")
  
  def setUp(self):
    self.input_file = None
    
  def tearDown(self):
    if self.input_file is not None and os.path.exists(self.input_file):
      os.unlink(self.input_file)
      
  def generateInputFile(self, commands):
    fd, self.input_file = tempfile.mkstemp()
    os.close(fd)
    f = open(self.input_file, "w")
    f.write("\n".join(commands))
    f.close()
    
  def runGame(self, options=[]):
    cmd_line = [self.getExeName()]
    cmd_line.extend(options)
    result = subprocess.call(cmd_line, stdin=open(self.input_file))
    self.assertEquals(0, result)
  
  def testQuitGame(self):
    commands = ["invalid cmd", "QuIt"]
    self.generateInputFile(commands)
    self.runGame()
    
  def testSaveGame(self):
    fd, save_file_name = tempfile.mkstemp()
    os.close(fd)
    commands = ["save %s" % save_file_name, "q"]
    self.generateInputFile(commands)
    self.runGame()
    output_lines = open(save_file_name).readlines()
    self.assertEquals(["256", "50", "0"], [x.strip() for x in output_lines])
    
  def testFullSixMenMorrisGame(self):
    folder = os.path.abspath(os.path.dirname(__file__))
    test_game_file = os.path.join(folder, "../game/test_games/full_6.txt")
    lines = open(test_game_file).readlines()
    lines = [line.strip().split() for line in lines if line[0] != '#']
    lines = [line[2:] for line in lines if len(line) > 1]
    for i in range(len(lines)):
      if len(lines[i]) == 2:
        lines[i] = chr(ord('A') + int(lines[i][0])) + chr(ord(lines[i][1]) + 1)
      elif len(lines[i]) == 4:
        lines[i] = (chr(ord('A') + int(lines[i][0])) + chr(ord(lines[i][1]) + 1)
            + chr(ord('A') + int(lines[i][2])) + chr(ord(lines[i][3]) + 1))
      else:
        raise ValueError(lines[i])
    self.generateInputFile(lines)
    self.runGame(["--game-type=6"])
    
  def testDummyNineMenMorrisActions(self):
    commands = ["invalid cmd", "help", "a", "a1", "g7", "quit"]
    self.generateInputFile(commands)
    self.runGame()
    
  def testClosedStdin(self):
    commands = []
    self.generateInputFile(commands)
    self.runGame()
    
  def testUsage(self):
    commands = []
    self.generateInputFile(commands)
    self.runGame(["--help"])

  def testFullNineMenMorrisGameWithAIPlayers(self):
    # TODO(python tests): Refactor the need to provide empty command file
    commands = []
    self.generateInputFile(commands)
    self.runGame(["--game-type=9",
                  "--white-player=random",
                  "--black-player=random"])
    
