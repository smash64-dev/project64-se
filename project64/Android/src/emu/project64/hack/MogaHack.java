/****************************************************************************
*                                                                           *
* Project64 - A Nintendo 64 emulator.                                       *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2016 Project64. All rights reserved.                        *
* Copyright (C) 2013 Paul Lamb
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
package emu.project64.hack;

import java.util.List;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ResolveInfo;
import android.content.pm.ServiceInfo;
import android.util.Log;

import com.bda.controller.Controller;
import com.bda.controller.IControllerService;

import emu.project64.AndroidDevice;

/**
 * Temporary hack for crash in MOGA library on Lollipop. This hack can be removed once MOGA fixes
 * their library. The actual issue is caused by the use of implicit service intents, which are
 * illegal in Lollipop, as seen in the logcat message below.
 * 
 * <pre>
 * {@code Service Intent must be explicit: Intent { act=com.bda.controller.IControllerService } }
 * </pre>
 * 
 * @see <a href="http://www.mogaanywhere.com/developers/">MOGA developer site</a>
 * @see <a href="http://commonsware.com/blog/2014/06/29/dealing-deprecations-bindservice.html">
 *      Discussion on explicit intents</a>
 */
public class MogaHack
{
    public static void init( Controller controller, Context context )
    {
        if( AndroidDevice.IS_LOLLIPOP )
        {
            boolean mIsBound = false;
            java.lang.reflect.Field fIsBound = null;
            android.content.ServiceConnection mServiceConnection = null;
            java.lang.reflect.Field fServiceConnection = null;
            try
            {
                Class<?> cMogaController = controller.getClass();
                fIsBound = cMogaController.getDeclaredField( "mIsBound" );
                fIsBound.setAccessible( true );
                mIsBound = fIsBound.getBoolean( controller );
                fServiceConnection = cMogaController.getDeclaredField( "mServiceConnection" );
                fServiceConnection.setAccessible( true );
                mServiceConnection = ( android.content.ServiceConnection ) fServiceConnection.get( controller );        
            }
            catch( NoSuchFieldException e )
            {
                Log.e( "MogaHack", "MOGA Lollipop Hack NoSuchFieldException (get)", e );
            }
            catch( IllegalAccessException e )
            {
                Log.e( "MogaHack", "MOGA Lollipop Hack IllegalAccessException (get)", e );
            }
            catch( IllegalArgumentException e )
            {
                Log.e( "MogaHack", "MOGA Lollipop Hack IllegalArgumentException (get)", e );
            }
            if( ( !mIsBound ) && ( mServiceConnection != null ) )
            {
                // Convert implicit intent to explicit intent, see http://stackoverflow.com/a/26318757
                Intent intent = new Intent( IControllerService.class.getName() );
                List<ResolveInfo> resolveInfos = context.getPackageManager().queryIntentServices( intent, 0 );
                if( resolveInfos == null || resolveInfos.size() != 1 )
                {
                    Log.e( "MogaHack", "Somebody is trying to intercept our intent. Disabling MOGA controller for security." );
                    return;
                }
                ServiceInfo serviceInfo = resolveInfos.get( 0 ).serviceInfo;
                String packageName = serviceInfo.packageName;
                String className = serviceInfo.name;
                intent.setComponent( new ComponentName( packageName, className ) );
                
                // Start the service explicitly
                context.startService( intent );
                context.bindService( intent, mServiceConnection, 1 );
                try
                {
                    fIsBound.setBoolean( controller, true );
                }
                catch( IllegalAccessException e )
                {
                    Log.e( "MogaHack", "MOGA Lollipop Hack IllegalAccessException (set)", e );
                }
                catch( IllegalArgumentException e )
                {
                    Log.e( "MogaHack", "MOGA Lollipop Hack IllegalArgumentException (set)", e );
                }
            }
        }
        else
        {
            controller.init();
        }
    }
}
