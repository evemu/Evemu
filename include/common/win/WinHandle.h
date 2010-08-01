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

#ifndef __WIN__WIN_HANDLE_H__INCL__
#define __WIN__WIN_HANDLE_H__INCL__

/**
 * @brief Wrapper around Windows' @c HANDLE.
 *
 * @author Bloody.Rabbit
 */
class WinHandle
{
public:
    /**
     * @brief Primary constructor.
     *
     * This constructor takes ownership of the handle,
     * i.e. closes it on destruction.
     *
     * @param[in] handle The handle.
     */
    WinHandle( HANDLE handle = INVALID_HANDLE );
    /**
     * @brief Copy constructor, duplicates the handle.
     *
     * @param[in] oth The handle to duplicate.
     */
    WinHandle( const WinHandle& oth );
    /**
     * @brief A destructor, closes the handle.
     */
    ~WinHandle();

    /**
     * @brief Checks if the handle is valid.
     *
     * @retval TRUE  The handle is valid.
     * @retval FALSE The handle is invalid.
     */
    BOOL isValid() const { return ( INVALID_HANDLE == mHandle ? FALSE : TRUE ); }

    /**
     * @brief Convenience cast to bool, calls isValid().
     *
     * @return A value returned by isValid().
     */
    operator BOOL() const { return isValid(); }
    /**
     * @brief Compares two handles for equality.
     *
     * @param[in] oth The other handle in comparison.
     *
     * @retval TRUE  The handles are equal.
     * @retval FALSE The handles are not equal.
     */
    BOOL operator==( const WinHandle& oth ) const { return ( oth.mHandle == mHandle ? TRUE : FALSE ); }

    /**
     * @brief Closes the handle.
     *
     * @return A value returned by @c CloseHandle.
     */
    BOOL Close();

    /**
     * @brief Copy operator, duplicates the handle.
     *
     * @param[in] oth The handle to duplicate.
     *
     * @return Itself.
     */
    WinHandle& operator=( const WinHandle& oth );

protected:
    /// The handle itself.
    HANDLE mHandle;
};

/**
 * @brief A WinHandle which you can wait for.
 *
 * @author Bloody.Rabbit
 */
class WinWaitableHandle
: public WinHandle
{
public:
    /**
     * @brief Primary constructor.
     *
     * @param[in] handle The handle.
     */
    WinWaitableHandle( HANDLE handle = INVALID_HANDLE );

    /**
     * @brief Waits for the handle.
     *
     * @param[in] timeout The wait timeout.
     *
     * @return A value returned by @c WaitForSingleObject.
     */
    DWORD Wait( DWORD timeout = INFINITE );
};

#endif /* !__WIN__WIN_HANDLE_H__INCL__ */