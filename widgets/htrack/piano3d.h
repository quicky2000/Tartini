/***************************************************************************
                          piano3d.h  -  description
                             -------------------
    begin                : 31 March 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef PIANO3D_H
#define PIANO3D_H

#include <vector>
#include <stdio.h>
#include <QGLWidget>
#include "myassert.h"

class Piano3d
{
  public:
    Piano3d( int p_num_keys = 85
           , int p_first_key = 21
           );
    ~Piano3d();

    static void drawWhiteKey();
    static void drawBlackKey();
  
    void draw();
  
    /** Initialises the piano.
        This can be called again to reinitialise the piano to a different size
        @param p_num_keys The number of keys the piano is to have
        @param p_first_key The midi note number that will be the lowest key on the piano
    */
    void init(int p_num_keys = 85, int p_first_key = 21);
  
    void setMaterialWhiteKey();
    void setMaterialBlackKey();

  
    /**
       @return The distance the in mm away from where the 0 midi note would be
    */
    double offsetAtKey(int p_key_num);
    inline bool keyState(int p_key_num);
    inline bool midiKeyState(int p_key_num);
    inline void setKeyState( int p_key_num
                           , bool p_state
                           );
    inline void setMidiKeyState( int p_key_num
                               , bool p_state
                               );
    void setAllKeyStatesOff();

    /**
       @return The width of the current piano keyboard in mm
    */
    double pianoWidth();
    inline int numKeys();
    inline int firstKey();
    inline const double & get_first_key_offset() const;
    float octaveWidth();

  private:
    static constexpr float m_white_key_width            = 23.5f;
    static constexpr float m_white_key_length           = 148.0f;
    static constexpr float m_white_key_length_inside    = 142.0f;
    static constexpr float m_white_key_height           = 16.0f;
    static constexpr float m_black_key_width            = 11.0f;
    static constexpr float m_black_key_length_top       = 94.0f;
    static constexpr float m_black_key_length_bottom    = 100.0f;
    static constexpr float m_black_key_height           = 10.0f;
    static constexpr float m_octave_width               = 164.5f;

    double m_first_key_offset;

    std::vector<bool> m_key_states;
    std::vector<float> m_key_offsets;
    int m_num_keys;
    /**
       on the midi scale (ie middle C = 60)
    */
    int m_first_key;

    /**
       aWhiteKeyLine;
    */
    GLint m_a_white_key;
    GLint m_a_black_key;
};

inline void setMaterialColor( float p_r
                            , float p_g
                            , float p_b
                            );
inline void setMaterialSpecular( float p_r
                               , float p_g
                               , float p_b
                               , float p_shiney
                               );
inline void setLightPosition( float p_x
                            , float p_y
                            , float p_z
                            );
inline void setLightDirection( float p_x
                             , float p_y
                             , float p_z
                             );
inline void setLightAmbient( float p_r
                           , float p_g
                           , float p_b
                           );
inline void setLightDiffuse( float p_r
                           , float p_g
                           , float p_b
                           );
inline void setLightSpecular( float p_r
                            , float p_g
                            , float p_b
                            );

#include "piano3d.hpp"

#endif // PIANO3D_H
// EOF
