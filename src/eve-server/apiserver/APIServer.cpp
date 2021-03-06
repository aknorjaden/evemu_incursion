/*
	------------------------------------------------------------------------------------
	LICENSE:
	------------------------------------------------------------------------------------
	This file is part of EVEmu: EVE Online Server Emulator
	Copyright 2006 - 2011 The EVEmu Team
	For the latest information visit http://evemu.org
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
	Author:		Aknor Jaden, adapted from ImageServer.h authored by caytchen
*/

#include "EVEServerPCH.h"
#include <iostream>
#include <fstream>

const char *const APIServer::FallbackURL = "http://api.eveonline.com/";

const char *const APIServer::Categories[] = {
    "admin",
    "account",
	"char",
	"corp", 
	"eve",
	"map",
	"server" };

const uint32 APIServer::CategoryCount = 7;

APIServer::APIServer()
{
	std::stringstream urlBuilder;
	urlBuilder << "http://" << sConfig.net.apiServer << ":" << (sConfig.net.apiServerPort) << "/";
	_url = urlBuilder.str();
/*
	_basePath = sConfig.files.imageDir;
	if (_basePath[_basePath.size() - 1] != '/')
		_basePath += "/";

	CreateNewDirectory(_basePath);

	for (int i = 0; i < CategoryCount; i++)
		CreateNewDirectory(_basePath + Categories[i]);

	sLog.Log("image server", "our URL: %s", _url.c_str());
	sLog.Log("image server", "our base: %s", _basePath.c_str());
*/
}

/*
bool APIServer::CreateNewDirectory(std::string& path)
{
	return mkdir(path.c_str(), 777) == 0;
}

void ImageServer::ReportNewImage(uint32 accountID, std::tr1::shared_ptr<std::vector<char>> imageData)
{
	Lock lock(_limboLock);

	if (_limboImages.find(accountID) != _limboImages.end())
		_limboImages.insert(std::pair<uint32,std::tr1::shared_ptr<std::vector<char>>>(accountID, imageData));
	else
		_limboImages[accountID] = imageData;
}

void ImageServer::ReportNewCharacter(uint32 creatorAccountID, uint32 characterID)
{
	Lock lock(_limboLock);

	// check if we received an image from this account previously
	if (_limboImages.find(creatorAccountID) == _limboImages.end())
		return;

	// we have, so save it
	//std::ofstream stream;
	std::string path(GetFilePath(std::string("Character"), characterID, 512));
    FILE * fp = fopen(path.c_str(), "wb");
    
	//stream.open(path, std::ios::binary | std::ios::trunc | std::ios::out);
	std::tr1::shared_ptr<std::vector<char>> data = _limboImages[creatorAccountID];

    fwrite(&((*data)[0]), 1, data->size(), fp);
    fclose(fp);
    
	//std::copy(data->begin(), data->end(), std::ostream_iterator<char>(stream));
	//stream.flush();
	//stream.close();

	// and delete it from our limbo map
	_limboImages.erase(creatorAccountID);

	sLog.Log("image server", "saved image from %i as %s", creatorAccountID, path.c_str());
}
*/

std::tr1::shared_ptr<std::vector<char>> APIServer::GetXML(std::string& category, uint32 id, uint32 size)
{
	if (!ValidateCategory(category) || !ValidateSize(category, size))
		return std::tr1::shared_ptr<std::vector<char>>();

	//std::ifstream stream;
	//std::string path(GetFilePath(category, id, size));
    //FILE * fp = fopen(path.c_str(), "rb");
    //if (fp == NULL)
    //    return std::tr1::shared_ptr<std::vector<char>>();
    //fseek(fp, 0, SEEK_END);
    //size_t length = ftell(fp);
    //fseek(fp, 0, SEEK_SET);

	//stream.open(path, std::ios::binary | std::ios::in);
	// not found or other error
	//if (stream.fail())
	//	return std::tr1::shared_ptr<std::vector<char>>();

	// get length
	//stream.seekg(0, std::ios::end);
	//int length = stream.tellg();
	//stream.seekg(0, std::ios::beg);

	//std::tr1::shared_ptr<std::vector<char>> ret = std::tr1::shared_ptr<std::vector<char>>(new std::vector<char>());
	//ret->resize(length);

	// HACK
	//stream.read(&((*ret)[0]), length);
    //fread(&((*ret)[0]), 1, length, fp);

    TiXmlDocument retXml;
    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "" );
    retXml.LinkEndChild( decl );

    TiXmlElement * eveapi = new TiXmlElement( "eveapi" );
    retXml.LinkEndChild( eveapi );
    eveapi->SetAttribute( "version", "2" );

    TiXmlElement * currentTime = new TiXmlElement( "currentTime" );
    currentTime->LinkEndChild( new TiXmlText( "2011-07-09 23:58:10" ));
    eveapi->LinkEndChild( currentTime );

    TiXmlElement * error = new TiXmlElement( "error" );
    error->SetAttribute( "code", "203" );
    error->LinkEndChild( new TiXmlText( "Authentication failure." ));
    eveapi->LinkEndChild( error );

    TiXmlElement * cachedUntil = new TiXmlElement( "cachedUntil" );
    cachedUntil->LinkEndChild( new TiXmlText( "2011-07-10 00:05:10" ));
    eveapi->LinkEndChild( cachedUntil );

    TiXmlPrinter xmlPrinter;
    retXml.Accept( &xmlPrinter );

    std::string xmlString = xmlPrinter.CStr();

    // TODO: I need to get this std::string xmlString into a std::vector<char>
	std::tr1::shared_ptr<std::vector<char>> ret = std::tr1::shared_ptr<std::vector<char>>(new std::vector<char>());
	//ret->resize(xmlString.length());
    unsigned long len = xmlString.length();
    for(int i=0; i<xmlString.length(); i++)
        ret->push_back(xmlString.at(i));

	return ret;
}

/*
std::string ImageServer::GetFilePath(std::string& category, uint32 id, uint32 size)
{
    std::string extension = category == "Character" ? "jpg" : "png";

	// HACK: We don't have any other
	size = 512;

	std::stringstream builder;
	builder << _basePath << category << "/" << id << "_" << size << "." << extension;
	return builder.str();
}
*/

bool APIServer::ValidateSize(std::string& category, uint32 size)
{
/*
	if (category == "InventoryType")
		return size == 64 || size == 32;

	if (category == "Alliance")
		return size == 128 || size == 64 || size == 32;

	if (category == "Corporation")
		return size == 256 || size == 128 || size == 64 || size == 32;

	// Render and Character
	return size == 512 || size == 256 || size == 128 || size == 64 || size == 32;
*/
	return true;
}

bool APIServer::ValidateCategory(std::string& category)
{
	for (int i = 0; i < 5; i++)
		if (category == Categories[i])
			return true;
	return false;
}

std::string& APIServer::url()
{
	return _url;
}

void APIServer::Run()
{
    _ioThread = std::auto_ptr<asio::thread>(new asio::thread(std::tr1::bind(&APIServer::RunInternal, this)));
}

void APIServer::RunInternal()
{
    _io = std::auto_ptr<asio::io_service>(new asio::io_service());
	_listener = std::auto_ptr<APIServerListener>(new APIServerListener(*_io));
	_io->run();
}

APIServer::Lock::Lock(asio::detail::mutex& mutex)
	: _mutex(mutex)
{
	_mutex.lock();
}

APIServer::Lock::~Lock()
{
	_mutex.unlock();
}
