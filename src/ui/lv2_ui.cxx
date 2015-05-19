
#include "../avtk/avtk.hxx"

#include "lv2/lv2plug.in/ns/lv2core/lv2.h"
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

#include "../test_ui.hxx"

#include "../dsp/bitta.hxx"
#include "../dsp/della.hxx"
/*
#include "driva.hxx"
#include "ducka.hxx"
#include "filta.hxx"
#include "kuiza.hxx"
#include "masha.hxx"
#include "panda.hxx"
*/
#include "roomy.hxx"
#include "../dsp/roomy.hxx"
/*
#include "satma.hxx"
#include "vihda.hxx"
#include "whaaa.hxx"
*/

static LV2UI_Handle avtk_instantiate(const struct _LV2UI_Descriptor * descriptor,
                              const char * plugin_uri,
                              const char * bundle_path,
                              LV2UI_Write_Function write_function,
                              LV2UI_Controller controller,
                              LV2UI_Widget * widget,
                              const LV2_Feature * const * features)
{
  LV2UI_Resize* resize = NULL;
  PuglNativeWindow parentHandle = 0;
  for (int i = 0; features[i]; ++i)
  {
    printf("Feature %s\n", features[i]->URI );
    if (!strcmp(features[i]->URI, LV2_UI__parent)) {
      parentHandle = (PuglNativeWindow)features[i]->data;
      printf("\tParent X11 ID %i\n", parentHandle );
    } else if (!strcmp(features[i]->URI, LV2_UI__resize)) {
      resize = (LV2UI_Resize*)features[i]->data;
    }
  }

  Avtk::UI* t = 0x0;
  
  /// Create the UI based on the URI
  if (strcmp(plugin_uri, "http://www.openavproductions.com/artyfx#roomy") == 0)
  {
    t = new RoomyUI( parentHandle );
  }
  else if (strcmp(plugin_uri, "http://www.openavproductions.com/artyfx#della") == 0 )
  {
    t = new TestUI( parentHandle );
  }
  else
  {
    fprintf(stderr, "ARTYFX_ROOMY error: this GUI does not support plugin with URI %s\n", plugin_uri);
    return NULL;
  }
  
  //t = new TestUI( parentHandle );
  
  //t->lv2_write_function = write_function;
  //t->lv2_controller     = controller;
  
  *widget = (void*)t->getNativeHandle();
  
  printf("init() - returning\n");
  
  if (resize) {
    resize->ui_resize(resize->handle, t->w(), t->h() );
  }
  
  return t;
}

static void avtk_cleanup(LV2UI_Handle ui)
{
  printf("cleanup()\n");
  delete (Avtk::UI*)ui;
}

static void avtk_port_event(LV2UI_Handle handle,
               uint32_t port_index,
               uint32_t buffer_size,
               uint32_t format,
               const void * buffer)
{
  Avtk::UI* ui = (Avtk::UI*)handle;
  //printf("port()\n");
  ui->lv2PortEvent( port_index, buffer_size, format, buffer);
  ui->redraw();
}

static int avtk_idle(LV2UI_Handle handle)
{
  //printf("idle()\n");
  Avtk::UI* ui = (Avtk::UI*)handle;
  ui->idle();
  return 0;
}

static const LV2UI_Idle_Interface idle_iface = { avtk_idle };

static const void*
avtk_extension_data(const char* uri)
{
	if (!strcmp(uri, LV2_UI__idleInterface)) {
		return &idle_iface;
	}
	return NULL;
}

static const LV2UI_Descriptor descriptor[] = {
{
  BITTA_UI_URI,
  avtk_instantiate,
  avtk_cleanup, 
  avtk_port_event, 
  avtk_extension_data
}, {
  DELLA_UI_URI,
  avtk_instantiate,
  avtk_cleanup, 
  avtk_port_event, 
  avtk_extension_data
}, {
  ROOMY_UI_URI,
  avtk_instantiate,
  avtk_cleanup, 
  avtk_port_event, 
  avtk_extension_data
} 
};

LV2_SYMBOL_EXPORT const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index)
{
  // bit of a hack - to handle multiple UIs at once
  //if( index >= 0 && index < 2 )
  {
		return &descriptor[index];
  }
  return NULL;
}

