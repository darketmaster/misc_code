#include <stdio.h>
#include <math.h>

#include "allegro.h"
#include "log.h"
#include "mdf.h"
#include "models.h"
#include "3ds.h"
#include "gamevar.h"

#define modelname1 "data/models/bolt/tris.md2"

void test_old()
{ /* 
  _obj3d mod3d;
  mod3d.load(modelname1);
  mod3d.save_mdf("modelmdf.bin");*/
/*  
  MD2MODEL md2mod(modelname1);
  md2mod.debug();*/
/*  
  MDFMODEL mdfmod,mdfmod2;
  
  mdfmod.load_md2(modelname1);
  mdfmod.save_mdf("bolt.mdf");
  mdfmod.debug();
  
  mdfmod2.load_mdf("bolt.mdf");  
  mdfmod2.debug();*/
  
  MDF_OBJ mdfobj,mdfobj2;
  mdfobj.import_md2(modelname1);
  mdfobj.save_mdf("bolt.mdf");  
  mdfobj.calculate_interpoled(0, 1, 0, 0);   
  mdfobj.debug_mdf_obj();
/*  
  mdfobj2.import_md2(modelname1);
  mdfobj2.load_mdf("bolt.mdf");
  mdfobj2.save_mdf("bolt_2.mdf");   
  mdfobj2.calculate_interpoled(0, 1, 0, 0);  
  mdfobj2.debug_mdf_obj();*/
}

#define NUMPATHS 5

char *namepaths[] = {"DATA",
                     "MODELS",
                     "SCENARIO",
                     "SOUND",
                     "MUSIC"};
void test()
{
  Gamepaths gamepaths;
  FLISTDATA flist;
  int i;
  
  gamepaths.create(NUMPATHS);
  for(i=0; i<NUMPATHS; i++)
      gamepaths.add(i, namepaths[i]);      
  gamepaths.load_fron_cfg_file("cfg/fpaths.cfg");  
  gamepaths.save("gamepaths.txt");
  
  flist.create("data\\", "*.*", FA_DIREC);// //FA_ARCH
  flist.save("txt.txt");       
}

int main(void)
{
  allegro_init();

  create_file_log("docs.log");
  
  test_old();
  //test();
  
   return 0;
}
END_OF_MAIN();

