// - AUTHOR: Hector Alvarez-Pol 01/2006
/******************************************************************
 * Copyright (C) 2005-2016, Hector Alvarez-Pol                     *
 * All rights reserved.                                            *
 *                                                                 *
 * License according to GNU LESSER GPL (see lgpl-3.0.txt).         *
 * For the list of contributors see CREDITS.                       *
 ******************************************************************/

#ifndef ActarSimVisManager_h
#define ActarSimVisManager_h 1


#include "G4VisManager.hh"

class ActarSimVisManager: public G4VisManager {
private:
  void RegisterGraphicsSystems ();
public:
  ActarSimVisManager ();
};
#endif
