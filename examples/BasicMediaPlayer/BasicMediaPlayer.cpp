/**********************************************************\ 
Original Author: Richard Bateman and Georg Fritzsche 

Created:    December 3, 2009
License:    Dual license model; choose one of two:
            Eclipse Public License - Version 1.0
            http://www.eclipse.org/legal/epl-v10.html
            - or -
            GNU Lesser General Public License, version 2.1
            http://www.gnu.org/licenses/lgpl-2.1.html

Copyright 2009 Georg Fritzsche,
               Firebreath development team
\**********************************************************/

#include <boost/assign.hpp>
#include "BasicMediaPlayer.h"
#include "MediaPlayer.h"
#include "DOM/JSAPI_DOMWindow.h"

#include <windows.h>

BasicMediaPlayer::BasicMediaPlayer(FB::BrowserHostWrapper *host) 
  : m_host(host)
  , m_player()
  , m_valid(false)
  , m_version("")
{
	using FB::make_method;
	using FB::make_property;

	registerMethod  ("play",     make_method  (this, &BasicMediaPlayer::Play));
	registerMethod  ("stop",     make_method  (this, &BasicMediaPlayer::Stop));
	registerProperty("version",  make_property(this, &BasicMediaPlayer::Version));
	registerProperty("type",     make_property(this, &BasicMediaPlayer::Type));
	registerProperty("playlist", make_property(this, &BasicMediaPlayer::Playlist,
													 &BasicMediaPlayer::SetPlaylist));

	try 
	{
		m_player  = MediaPlayerPtr(new MediaPlayer);
		m_valid   = true;
	} 
	catch(MediaPlayer::InitializationException&) 
	{
		m_player.reset();
	}
}

BasicMediaPlayer::~BasicMediaPlayer()
{
	m_player.reset();
}

bool BasicMediaPlayer::Play()
{
	return false;
}

bool BasicMediaPlayer::Stop()
{
	return false;
}

std::string BasicMediaPlayer::Version()
{
	return m_player->Version();
}

std::string BasicMediaPlayer::Type()
{
	return m_player->Type();
}

FB::JSAPI* BasicMediaPlayer::Playlist()
{
	return 0;
}

void BasicMediaPlayer::SetPlaylist(FB::AutoPtr<FB::BrowserObjectAPI> obj)
{
	if(!obj->HasProperty("length"))
		throw FB::invalid_arguments("expected array parameter");
	
	FB::variant res = obj->GetProperty("length");
	long length = 0;
	try {
		length = res.convert_cast<long>();
	} catch(FB::bad_variant_cast&) {
		throw FB::script_error("length returned unexpected type");
	}

	std::vector<std::string> entries;

	try 
	{
		for(long i=0; i<length; ++i) 
		{
			res = obj->GetProperty(i);
			entries.push_back(res.convert_cast<std::string>());
		}
	}
	catch(FB::bad_variant_cast&)
	{
		throw FB::script_error("could not convert entry in playlist to string");
	}

	std::string s;
	for(std::vector<std::string>::iterator it = entries.begin(); it != entries.end(); ++it)
	{
		s += *it + "\n";
	}
	
	m_host->getDOMWindow().alert(s);
}