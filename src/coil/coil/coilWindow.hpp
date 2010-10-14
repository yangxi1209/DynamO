/*  DYNAMO:- Event driven molecular dynamics simulator 
    http://www.marcusbannerman.co.uk/dynamo
    Copyright (C) 2010  Marcus N Campbell Bannerman <m.bannerman@gmail.com>

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 3 as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class CoilWindow
{
protected:
  int          windowID;
public:
  
  CoilWindow();
  ~CoilWindow();
  
  virtual void CallBackDisplayFunc() {}
  virtual void CallBackIdleFunc() {}
  virtual void CallBackKeyboardFunc(unsigned char key, int x, int y) {}
  virtual void CallBackKeyboardUpFunc(unsigned char key, int x, int y) {}
  virtual void CallBackMotionFunc(int x, int y) {}
  virtual void CallBackMouseFunc(int button, int state, int x, int y) {}
  virtual void CallBackMouseWheelFunc(int button, int dir, int x, int y) {}
  virtual void CallBackPassiveMotionFunc(int x, int y) {}
  virtual void CallBackReshapeFunc(int w, int h) {}
  virtual void CallBackSpecialFunc(int key, int x, int y) {}
  virtual void CallBackSpecialUpFunc(int key, int x, int y) {}
  virtual void CallBackVisibilityFunc(int visible) {}
  
  void    SetWindowID(const int newWindowID) { windowID = newWindowID; }
  int     GetWindowID() { return windowID; }
  
  virtual void init() = 0;

  //If glut is closing the window through its window controls you
  //should not glutDestroyWindow, it will be called automatically.
  //Otherwise you should call it with deinit(true);
  virtual void deinit(bool andGlutDestroy = true) = 0;

  inline bool isReady() const { return _readyFlag; }
  
protected:
  volatile bool _readyFlag;

  virtual void initOpenGL() = 0;  
  virtual void initOpenCL() = 0;
  virtual void initGTK() = 0;
};
