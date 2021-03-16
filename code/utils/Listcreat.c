#include "allegro.h"
#include "spr.h"
#define NAME_INI "ini.ini"

#define STR_VERSION "List_spr creator v0.1 beta"

#define MAXNAMES 1000

int sw=640,sh=480;
int windowed = 1;

int numfiles=0;
char **fnames;

void free_fnames_mem()
{
 int i;
 if(fnames)
 {
   for(i=0;i<MAXNAMES;i++)
       if(fnames[i]) free(fnames[i]);
   free(fnames);
 }
}

int get_fname_mem()
{
 int ok=0;
 int i;
 fnames = (char**)calloc(MAXNAMES,sizeof(char*));
 if(fnames)
 {
    for(i=0;i<MAXNAMES;i++)
        fnames[i] = NULL;
    for(i=0;i<MAXNAMES;i++)
    {
        fnames[i] = (char *)calloc(128,sizeof(char));
        if(fnames[i]==NULL) {free_fnames_mem(); return ok;}
    }
    ok = 1;
 }
 return ok;
}
int load_config(char *fname)
{
 FILE *f;

 f=fopen(fname,"r");
 if (f==NULL) return 1;

 fscanf(f,"W:%d H:%d\n",&sw,&sh);
 fscanf(f,"Windowed = %d",&windowed);

 fclose(f);
 return 0;
}

void save_config(char *fname)
{
 FILE *f;

 f=fopen(fname,"w");
 if(f)
 {
 fprintf(f,"W:%d H:%d\n",sw,sh);
 fprintf(f,"Windowed = %d",windowed);

 fclose(f);
 }//end if f
}

int ini_liballeg()
{
   int gfxerror=0;
   int mode = GFX_AUTODETECT;
   allegro_init();
   install_timer();
   install_mouse();
   install_keyboard();

  if(load_config(NAME_INI))
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("ini no found \n tamaño estandar 640x480");
   }

   set_color_depth(16);
   if(windowed) mode = GFX_AUTODETECT_WINDOWED;

   if (set_gfx_mode(mode, sw, sh, 0, 0) != 0) {
       set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
       allegro_message("Unable to set any graphic mode\n%s\n", allegro_error);
       gfxerror=1;
   }

   return gfxerror;
}

#define TPCX 0
#define TBMP 1
#define TTGA 2
#define TSPR 3
#define TTXT 4
#define TUNKNOWN 9

int get_type(char *cad)
{
   if(strcmpi(cad,"PCX")==0)
      return TPCX;
   if(strcmpi(cad,"BMP")==0)
      return TBMP;
   if(strcmpi(cad,"TGA")==0)
      return TTGA;
   if(strcmpi(cad,"SPR")==0)
      return TSPR;
   if(strcmpi(cad,"TXT")==0)
      return TTXT;
   return TUNKNOWN;
}

void create_list(char *name)
{
   list_Spr *lspr=NULL;
   Spr *spr=NULL;
   BITMAP *bmp=NULL;
   int i;
   int type;
   PALLETE pal;

   lspr = create_list_Spr(numfiles);

   for(i=0;i<numfiles;i++)
   {
       type = get_type(get_extension(fnames[i]));
       switch(type)
       {
        case TPCX:
        case TBMP:
        case TTGA:
             set_color_conversion(COLORCONV_NONE);
             bmp = load_bitmap(fnames[i],pal);
             add_Spr_bmp(lspr,bmp);
             destroy_bitmap(bmp);
        break;
        case TSPR:
             spr = load_Spr(fnames[i]);
             add_Spr(lspr,spr);
        break;
        case TTXT:
             if(alert(get_filename(fnames[i]), "is a text file and will", "produce fail, continue?", "Yes", "NO", 121, 110)==1)
             {
             spr = load_Spr_txt(fnames[i]);
             add_Spr(lspr,spr);
             }
        break;
        case TUNKNOWN:
        break;
       }
   }

   save_list_Spr(lspr,name);

   destroy_list_Spr(lspr);
}

int main()
{
   BITMAP *buffer;
   int salir=0;
   int tempkey;
   char cadtemp[128];

   //save_config(NAME_INI);
   if(ini_liballeg())
      return 1;

   if(!get_fname_mem())
   {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("insuficiente memoria para la operacion");
      return 1;
    }

   /* allocate the memory buffer */
   buffer = create_bitmap(SCREEN_W, SCREEN_H);
   text_mode(-1);

   clear(buffer);
   textprintf(buffer, font, 0, 0, makecol(255,255,255), "press A for add spr");
   textprintf(buffer, font, 0, 8, makecol(255,255,255), "press N for New list");
   textprintf(buffer, font, 0, 16, makecol(255,255,255), "press S for save listspr data");
   textprintf(buffer, font, 0, 24, makecol(255,255,255), "press ESC for EXIT");
   textprintf(buffer, font, 0, SCREEN_H-8, makecol(255,255,255), STR_VERSION);
   blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

   while(!salir) {

        if(keypressed())
        {
           tempkey = readkey() >> 8;
           switch(tempkey)
           {
            case KEY_ESC:
                 if(alert("Exit?", "Sure?", NULL, "Yes", "NO", 121, 110)==1)
                    salir = 1;
            break;
            case KEY_A:
                 sprintf(cadtemp,"\0");
                 if(file_select("new data to add BMP;PCX;TGA;SPR;TXT", cadtemp, "BMP;PCX;TGA;SPR;TXT"))
                 {
                  sprintf(fnames[numfiles],cadtemp);
                  numfiles++;
                 }
            break;
            case KEY_N:
                 if(alert("New list?", "Sure?", NULL, "Yes", "NO", 121, 110)==1)
                 numfiles = 0;
            break;
            case KEY_S:
                 if(numfiles>0)
                 {
                    sprintf(cadtemp,"\0");
                    if(file_select("Save list lsp , txt", cadtemp, "lsp;txt"))
                    {
                       if(exists(cadtemp))
                       { if(alert(get_filename(cadtemp),"exist", "Overwrite?", "Yes", "NO", 121, 110)==1)
                         {
                            create_list(cadtemp);
                            alert(get_filename(cadtemp), "susceful", "save", "OK", "CANCEL", 121, 110);
                         }
                       }//end if existi
                       else {
                              create_list(cadtemp);
                              alert(get_filename(cadtemp), "susceful", "save", "OK", "CANCEL", 121, 110);
                            }
                     }//if file select
                 }//if numfiles >0
            break;
           }

        clear(buffer);
        textprintf(buffer, font, 0, 0, makecol(255,255,255), "press A for add spr");
        textprintf(buffer, font, 0, 8, makecol(255,255,255), "press N for New list");
        textprintf(buffer, font, 0, 16, makecol(255,255,255), "press S for save listspr data");
        textprintf(buffer, font, 0, 24, makecol(255,255,255), "press ESC for EXIT");
        if(numfiles>0)
        textprintf(buffer, font, 0, SCREEN_H-16, makecol(255,255,255), "last: %s",fnames[numfiles-1]);
        textprintf(buffer, font, 0, SCREEN_H-24, makecol(255,255,255), "num data in list: %d",numfiles);
        textprintf(buffer, font, 0, SCREEN_H-8, makecol(255,255,255), STR_VERSION);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        }
   }

   free_fnames_mem();
   destroy_bitmap(buffer);
   set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
   allegro_message("version: %s",STR_VERSION);

   return 0;
}END_OF_MAIN();
