/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of EVEmu: EVE Online Server Emulator
    Copyright 2006 - 2008 The EVEmu Team
    For the latest information visit http://evemu.mmoforge.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
    Author:     Bloody.Rabbit
*/

#include "CommonPCH.h"

#include "mt/ThreadMgr.h"
#include "time/TimeMgr.h"
#include "util/Log.h"

/*************************************************************************/
/* Time::TimeMgr                                                         */
/*************************************************************************/
Time::TimeMgr::TimeMgr( size_t period )
: mRun( true ),
  mRunTimer( period )
{
    // do a single update ourselves
    Update();
    // start the update thread
    sThreadMgr.Run( this );
}

Std::Tm Time::TimeMgr::nowTm() const
{
    Std::Tm t;

    {
        Mt::MutexLock lock( mMutex );
        t = mTm;
    }

    return t;
}

Time::WinTime Time::TimeMgr::nowWin() const
{
    WinTime wt;

    {
        Mt::MutexLock lock( mMutex );

#   ifdef WIN32
        wt = mWinTime;
#   else /* !WIN32 */
        wt = mTimeval + Timeval::SEC * EPOCH_DIFF_SEC;
#   endif /* !WIN32 */
    }

    return wt;
}

Time::Timeval Time::TimeMgr::nowUnix() const
{
    Timeval tv;

    {
        Mt::MutexLock lock( mMutex );

#   ifdef WIN32
        tv = mWinTime - WinTime::SEC * EPOCH_DIFF_SEC;
#   else /* !WIN32 */
        tv = mTimeval;
#   endif /* !WIN32 */
    }

    return tv;
}

void Time::TimeMgr::Update()
{
    Mt::MutexLock lock( mMutex );

#ifdef WIN32
    mWinTime = WinTime::now();
    mTm = Std::Tm::now();
#else /* !WIN32 */
    mTimeval = Timeval::now();
    mTm = mTimeval.sec();
#endif /* !WIN32 */
}

void Time::TimeMgr::Run()
{
    Mt::MutexLock lock( mMutex );

    mRunTimer.Start();
    while( mRun )
    {
        // update the time stuff
        Update();

        // wait for the next update time
        lock.Unlock();
        mRunTimer.Sleep();
        lock.Relock();
    }
}
