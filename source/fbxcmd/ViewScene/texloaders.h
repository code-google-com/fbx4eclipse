#pragma once

class VSTexture;

//! A function for loading textures.
/*!
* Loads a texture pointed to by filepath.
* Returns true on success and false with lError populated otherwise.
*
* \param filepath The full path to the texture that must be loaded.
* \param texture Contains the texture in KTexture format on successful load.
* \param lError Contains the error information when file cannot be loaded.
* \return true if the load was successful, false otherwise.
*/
extern bool LoadTexture( const KString & lFileName, VSTexture * lTexture, KError & lError );
