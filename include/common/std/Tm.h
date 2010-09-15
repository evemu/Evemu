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

#ifndef __STD__TM_H__INCL__
#define __STD__TM_H__INCL__

namespace Std
{
    /**
     * @brief A wrapper around <code>struct tm</code>.
     *
     * @author Bloody.Rabbit
     */
    class Tm
    {
    public:
        /**
         * @brief Obtains current time.
         *
         * This method simply calls a method <code>time</code>,
         * then uses the conversion constructor; do not expect
         * it to do anything else.
         *
         * You should not use this method at all anyway - see
         * Time::TimeMgr if you want to obtain current time.
         *
         * @return The time.
         */
        static Tm now();

        /// A default constructor.
        Tm();
        /**
         * @brief A primary constructor.
         *
         * @param[in] t A <code>tm</code>.
         */
        Tm( const tm& t );
        /**
         * @brief A conversion operator.
         *
         * @param[in] t A <code>time_t</code> value to convert.
         */
        Tm( time_t t );

        /**
         * @brief A copy operator.
         *
         * @param[in] oth An object to copy.
         */
        Tm( const Tm& oth );

        /// Obtains the stored <code>tm</code>.
        const tm& t() const { return mTm; }

        /// Obtains a number of seconds after the minute.
        int sec() const { return t().tm_sec; }
        /// Obtains a number of minutes after the hour.
        int min() const { return t().tm_min; }
        /// Obtains a number of hours since midnight.
        int hour() const { return t().tm_hour; }

        /// Obtains a day of the month.
        int mday() const { return t().tm_mday; }
        /// Obtains a number of days since Sunday.
        int wday() const { return t().tm_wday; }
        /// Obtains a number of days since January 1.
        int yday() const { return t().tm_yday; }

        /// Obtains a number of months since January.
        int mon() const { return t().tm_mon; }
        /// Obtains a number of years since 1900.
        int year() const { return t().tm_year; }

        /// Obtains the daylight saving time flag.
        int isDst() const { return t().tm_isdst; }

    protected:
        /// The stored <code>tm</code>.
        tm mTm;
    };
}

#endif /* !__STD__TM_H__INCL__ */
