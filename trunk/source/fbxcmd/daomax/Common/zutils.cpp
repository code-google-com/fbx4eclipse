/**********************************************************************
*<
FILE: NIUtils.cpp

DESCRIPTION:	MDBImporter Utilities

CREATED BY: tazpn (Theo)

HISTORY: 

*>	Copyright (c) 2006, All Rights Reserved.
**********************************************************************/
#include "pch.h"
#include "niutils.h"
#include "zlib/unzip.h"
//#include "zlib/iowin32.h"

struct vbuf
{
   vbuf(size_t s) : size(s), buffer( calloc(1, size) ) {}
   ~vbuf() { free(buffer); }
   size_t size;
   void *buffer;
};

static 
voidpf ZCALLBACK buf_fopen_file_func(voidpf opaque,const char* filename,int mode)
{
   FILE* file = NULL;
   const char* mode_fopen = NULL;
   if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER)==ZLIB_FILEFUNC_MODE_READ)
      mode_fopen = "rb";
   else
      if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
         mode_fopen = "r+b";
      else
         if (mode & ZLIB_FILEFUNC_MODE_CREATE)
            mode_fopen = "wb";

   if ((filename!=NULL) && (mode_fopen != NULL))
      file = fopen(filename, mode_fopen);
   if (file != NULL && opaque)
   {
      vbuf* fbuf = (vbuf*)opaque;
      setvbuf(file, (char*)fbuf->buffer, _IOFBF, fbuf->size);
   }
   return file;
}

static
uLong ZCALLBACK buf_fread_file_func(voidpf opaque,voidpf stream,void* buf,uLong size)
{
   return (uLong)fread(buf, 1, (size_t)size, (FILE *)stream);
}

static 
uLong ZCALLBACK buf_fwrite_file_func(voidpf opaque,voidpf stream,const void* buf,uLong size)
{
   return (uLong)fwrite(buf, 1, (size_t)size, (FILE *)stream);
}

static 
long ZCALLBACK buf_ftell_file_func(voidpf opaque, voidpf stream)
{
   return ftell((FILE *)stream);
}

static 
long ZCALLBACK buf_fseek_file_func(voidpf opaque,voidpf stream,uLong offset,int origin)
{
   int fseek_origin=0;
   long ret;
   switch (origin)
   {
   case ZLIB_FILEFUNC_SEEK_CUR :
      fseek_origin = SEEK_CUR;
      break;
   case ZLIB_FILEFUNC_SEEK_END :
      fseek_origin = SEEK_END;
      break;
   case ZLIB_FILEFUNC_SEEK_SET :
      fseek_origin = SEEK_SET;
      break;
   default: return -1;
   }
   ret = 0;
   fseek((FILE *)stream, offset, fseek_origin);
   return ret;
}

static 
int ZCALLBACK buf_fclose_file_func(voidpf opaque,voidpf stream)
{
   return fclose((FILE *)stream);
}

static 
int ZCALLBACK buf_ferror_file_func(voidpf opaque,voidpf stream)
{
   int ret;
   ret = ferror((FILE *)stream);
   return ret;
}

static 
void fill_buf_fopen_filefunc (zlib_filefunc_def* pzlib_filefunc_def)
{
   pzlib_filefunc_def->zopen_file = buf_fopen_file_func;
   pzlib_filefunc_def->zread_file = buf_fread_file_func;
   pzlib_filefunc_def->zwrite_file = buf_fwrite_file_func;
   pzlib_filefunc_def->ztell_file = buf_ftell_file_func;
   pzlib_filefunc_def->zseek_file = buf_fseek_file_func;
   pzlib_filefunc_def->zclose_file = buf_fclose_file_func;
   pzlib_filefunc_def->zerror_file = buf_ferror_file_func;
   pzlib_filefunc_def->opaque = NULL;
}

bool ExtractFile(const char* zipfilename, const char *filename, const char *dest)
{
   bool ok = false;
   vbuf fbuf(0x10000);
   zlib_filefunc_def ffunc;
   //fill_win32_filefunc(&ffunc);
   fill_buf_fopen_filefunc(&ffunc);
   ffunc.opaque = (voidpf)&fbuf;

   unzFile uf = unzOpen2(zipfilename,&ffunc);
   if (UNZ_OK == unzLocateFirstWildCard(uf, filename))
   {
      char filename_inzip[MAX_PATH];
      unz_file_info file_info;
      uLong ratio=0;
      if (UNZ_OK == unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0))
      {
         uInt size_buf = 0x2000;
         void* buf = (void*)malloc(size_buf);

         if (UNZ_OK == unzOpenCurrentFilePassword(uf,NULL))
         {
            FILE* fout=fopen(dest, "wb");
            int err=UNZ_OK;
            do
            {
               err = unzReadCurrentFile(uf,buf,size_buf);
               if (err > 0){
                  if (fwrite(buf,err,1,fout)!=1) {
                     err = UNZ_ERRNO;
                     break;
                  }
               }
            } while (err > 0);
            fclose(fout);
            ok = true;
         }
         free(buf);
      }
   }
   unzCloseCurrentFile(uf);
   return ok;
}
