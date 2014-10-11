/*
* Copyright (c) 2014 David Wicks, sansumbrella.com
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or
* without modification, are permitted provided that the following
* conditions are met:
*
* Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Motion.h"

using namespace choreograph;
using namespace std;

//=================================================
// TimelineItem
//=================================================

void TimelineItem::step( Time dt )
{
  _time += dt * _speed;
  update(); // update properties
  _previous_time = _time;
}

void TimelineItem::jumpTo( Time time )
{
  _time = time;
  update(); // update properties
  _previous_time = _time;
}

bool TimelineItem::isFinished() const
{
  if( backward() ) {
    return time() <= 0.0f;
  }
  else {
    return time() >= getDuration();
  }
}

void TimelineItem::resetTime()
{
  if( forward() )
  {
    _time = _previous_time = 0.0f;
  }
  else
  {
    _time = _previous_time = getEndTime();
  }
}

//=================================================
// MotionGroup
//=================================================

void MotionGroup::update()
{
  if( _start_fn )
  {
    if( forward() && time() > 0.0f && previousTime() <= 0.0f ) {
      _start_fn( *this );
    }
    else if( backward() && time() < getDuration() && previousTime() >= getDuration() ) {
      _start_fn( *this );
    }
  }

  // Advance motions to current time.
  for( auto &motion : _motions ) {
    motion->jumpTo( time() );
  }

  if( _finish_fn )
  {
    if( forward() && time() >= getDuration() && previousTime() < getDuration() ) {
      _finish_fn( *this );
    }
    else if( backward() && time() <= 0.0f && previousTime() > 0.0f ) {
      _finish_fn( *this );
    }
  }
}

bool MotionGroup::isValid() const
{
  for( auto &motion : _motions ) {
    if( ! motion->isValid() ) {
      return false;
    }
  }

  return true;
}

//=================================================
// Cue
//=================================================

Cue::Cue( const function<void ()> &fn, Time delay ):
  _cue( fn ),
  _control( make_shared<Cue::Control>() )
{
  setStartTime( delay );
}

bool Cue::isValid() const
{
  return _control->_valid;
}

void Cue::update()
{
  if( isValid() )
  {
    if( forward() && time() >= 0.0f && previousTime() < 0.0f )
      _cue();
    else if( backward() && time() <= 0.0f && previousTime() > 0.0f )
      _cue();
  }
}
