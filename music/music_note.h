/***************************************************************************
                          music_note.h
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
    copyright            : (C) 2016 by Julien Thevenon
    email                : julien_thevenon at yahoo.fr
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef MUSICNOTES_H
#define MUSICNOTES_H

#include "useful.h"
#include "myassert.h"
#include <string>
#include "gdata.h"
#include "music_temperament.h"

/**
 Helper class for musical notes -- all members are static.
 
 Terminology:
 - A "note" is an integer value that represents a musical note, such as C4.  The values are MIDI note numbers, e.g. C4 = 60.
 - A "pitch" is a floating-point value that represents a musical pitch, also measured on the MIDI scale.
 
 The mapping between note numbers and pitches depends on the key and temperament.
 For example, the note G4 (66) in Pythagorean temperament (in the key of C) has a pitch of 66.0196, which is 1.96 cents higher than in even temperament.
 Only in even temperament do the note numbers correspond to the pitch values.
 - Use `temperedPitch()` to convert from a note number to the corresponding pitch.
 - Use `closestNote()` to convert from a pitch to the closest note number.
 
 You can pass the key and temperament explicitly or rely on the default values, which use the globally selected key and temperament in `GData`
 */
class MusicNote
{
  public:

    static
    const std::string & noteName(int p_note);

    /**
     The octave number of the note in Scientific Pitch Notation
     @param p_note The note number
     @return The octave number. Middle C (MIDI note 60) is defined as octave 4.  Note number 0 is in octave -1
    */
    static int noteOctave(int p_note);

    /**
    The semitone value of the note within its octave
    @param p_note The note number
    @return The semitone value (0..11), which corresponds to the note letters C, C♯, ..,, B♭, B.
    */
    static int semitoneValue(int p_note);

    /**
     The semitone value of the note, transposed into the given musical key
     @param p_note The note number
     @param p_music_key The musical key (0..11).  If not specified, defaults to the globally selected key: `GData::musicKey()`
     @return The semitone value (0..11), which corresponds to the note letters C, C♯, ..,, B♭, B.
    */
    static int semitoneValueInKey(int p_note
                                , int p_music_key = g_music_key_roots[GData::getUniqueInstance().musicKey()]
                                  );

    static double pitchOffsetInKey(const double & p_pitch, int p_music_key);

    /**
     The tempered pitch of a note, given the key and temperament
     @param p_note The note number
     @param p_music_key The musical key (0..11).  If not specified, defaults to the globally selected key: `GData::musicKey()`
     @param p_music_temperament The musical temperament.  If not specified, defaults to the globally selected temperament: `GData::musicTemperament()`
     @return The tempered pitch.  Returns 0 if the temperament does not include the note
    */
    static double temperedPitch(int p_note
                              , int p_music_key = g_music_key_roots[GData::getUniqueInstance().musicKey()]
                              , const MusicTemperament & p_music_temperament = MusicTemperament::getTemperament(GData::getUniqueInstance().musicTemperament())
                                );

    /**
     The closest note to a pitch, given the key and temperament
     @param p_pitch The pitch
     @param p_music_key The musical key (0..11).  If not specified, defaults to the globally selected key: `GData::musicKey()`
     @param p_music_temperament The musical temperament.  If not specified, defaults to the globally selected temperament: `GData::musicTemperament()`
     @return The note associated with the closest pitch in the temperament
    */
    static int closestNote(const double & p_pitch
                         , int p_music_key = g_music_key_roots[GData::getUniqueInstance().musicKey()]
                         , const MusicTemperament & p_music_temperament = MusicTemperament::getTemperament(GData::getUniqueInstance().musicTemperament())
                           );

    static
    void init_note_names();

    /**
       Converts the frequencies freq (in hertz) into their pitch on the midi scale
       i.e. the number of semi-tones above C0
       Note: The note's pitch will contain its fractional part
       Reference = http://www.borg.com/~jglatt/tutr/notenum.htm
       @param p_freq The frequency in Hz
       @return The pitch in fractional part semitones from the midi scale.
    */
    static inline double freq2pitch(const double & p_freq);

    /**
       Does the opposite of the function above
    */
    static inline double pitch2freq(const double & p_pitch);

    static void initMusicStuff();
    
    // Deprecated member functions (do not remove)
    
    /// @deprecated Use closestNote() to convert pitches to notes
    [[deprecated("Use closestNote() to convert pitches to notes")]]
    static inline const std::string & noteName(const double & p_pitch);

    /// @deprecated Use closestNote() to convert pitches to notes
    [[deprecated("Use closestNote() to convert pitches to notes")]]
    static inline int noteOctave(const double & p_pitch);

    /// @deprecated Use closestNote() to convert pitches to notes
    [[deprecated("Use closestNote() to convert pitches to notes")]]
    static inline int semitoneValue(const double & p_pitch);

    static bool isBlackNote(int p_note);
    /// @deprecated Use closestNote() to convert pitches to notes
    [[deprecated("Use closestNote() to convert pitches to notes")]]
    static inline bool isBlackNote(const double & p_pitch);

    private:
      static std::array<std::string, 12> m_note_names;
};

#include "music_note.hpp"

#endif // MUSICNOTES_H
//EOF
