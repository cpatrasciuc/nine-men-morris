#!/usr/bin/python
# Quick script that runs cpplint on all .cc or .h files before 'git commit'

import os
import sys

git_dir = os.popen("git rev-parse --show-cdup").readlines()[0][:-1]
cpplint_path = os.path.join(git_dir, "tools/cpplint.py")

files = os.popen("git diff --cached --name-only").readlines()
files = [os.path.join(git_dir, x[:-1]) for x in files]

cmd = "python '%s' --filter=-readability/streams,-readability/function,-readability/casting %s"
ret = os.system(cmd % (cpplint_path, " ".join(files)))
if ret != 0:
  print "Exit code: %s" % ret
  sys.exit(1)
