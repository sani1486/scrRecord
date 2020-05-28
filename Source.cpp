#include <gst/gst.h>
//#include "pch.h"

#include <windows.h> 
#include <stdio.h>
GMainLoop* mainLoop;
GstElement *mainPipeline;
GstPadLinkReturn link_to_mixer(GstPad* binPad, GstElement* mix);
GstPad* retrieve_ghost_pad(GstElement* bin, GstElement* elem);
typedef struct _elemStruct
{
	GstElement  *micSource, *micSourceQueue, *soundCardSrc, *soundCardSrcQueue, *micSrcRate, *micRateQueue, *soundCardRate, *soundCardRateQueue, *audioMixer, *audioMixerQueue;
	GstElement* audioConverter, *audioConverterQueue, *audioEncoder, *audioEncoderQueue, *avMuxer, *gdiGrabber, *videoConverter, *x264encoder;
	GstElement* muxerQueue, *fileSinker, *gdiGrabberQueue, *videoConverterQueue, *x264encoderQueue;
	GstCaps *caps;
	GstElement* message;
	GstStateChangeReturn stateRet;
	GstElement *micBin, *soundCardBin, *screenBin, *audioBin;
	GstPad *micMixPad, *soundCardMixPad, *audioMuxPad, *videoMuxPad;
	GstBus* mainBus;
	GstStateChangeReturn ret;
	GstMessage* msg;
	guint bus_watch_id;
	GstElement* soundCardTempSink;
}elemStruct;
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event\n\n");
		Beep(750, 300);
		return TRUE;

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		Beep(600, 200);
		printf("Ctrl-Close event\n\n");
		return TRUE;

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		Beep(1000, 200);
		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep(750, 500);
		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}
void addsoundsrc_toMainline(GstPadProbeInfo* info, GstElement* bin)
{
	// we got data , add pipeline to audiomixer 
	// add bin to audiomixer
	// get bin src pad 
	// call retrieve ghostsrc function 
	//retrieve_ghost_pad()
	GstElement* queue = gst_bin_get_by_name(GST_BIN(bin), "sound_card_source_queue");
	GstPad* mixpad = retrieve_ghost_pad(bin, queue);
	//link_to_mixer(mixpad, )

}
GstPadProbeReturn soundCardProbe(GstPad* pad, GstPadProbeInfo* info, gpointer data)
{
	//GstBuffer* buffer = gst_pad_probe_info_get_buffer(info);
	GstBuffer* buffer = GST_PAD_PROBE_INFO_BUFFER(info);
	elemStruct* mainElem = (elemStruct*)data;
	g_print("received data in the soundcard probe ");
	//GstElement* bin = mainElem->soundCardBin;
	//bool add = gst_bin_add(GST_BIN(mainElem->audioBin), mainElem->soundCardBin);
	//gst_element_sync_state_with_parent(mainElem->soundCardBin);
	//GstElement* queue = gst_bin_get_by_name((GST_BIN(bin)), "sound_card_source_queue");
	//GstPad* mixpad = retrieve_ghost_pad(bin, mainElem->soundCardSrcQueue);
	//GstPad* mixPad = gst_element_get_static_pad(mainElem->soundCardSrcQueue, "sink");

	//link_to_mixer(mixPad, mainElem->audioMixer);
	//addsoundsrc_toMainline(info, bin);
	return GST_PAD_PROBE_DROP;
}
void set_queue_property(GstElement* _queue)
{
	g_object_set(G_OBJECT(_queue), "max-size-buffers", 1000, "max-size-time", 1000000000000, NULL);
}

GstPadLinkReturn link_to_mixer(GstPad* binPad, GstElement* mix)
{
	GstPad* mixerPad;
	gchar* binPadName, *mixerPadName;
	mixerPad = gst_element_get_compatible_pad(mix, binPad, NULL);
	//mixerPad = gst_element_get_request_pad(mix, "sink_%u");
	binPadName = gst_pad_get_name(binPad);
	mixerPadName = gst_pad_get_name(mixerPad);
	GstPadLinkReturn retVal = gst_pad_link(binPad, mixerPad);  // check if succesfull;
	g_print(" a new link is creatd with %s and %s pads\n", binPadName, mixerPadName);
	g_free(binPadName);
	g_free(mixerPadName);
	//gst_object_unref(binPad);
	gst_object_unref(mixerPad);
	//gst_element_release_request_pad(mix, mixerPad);
	return retVal;
}

GstPadLinkReturn audio_link_to_muxer(GstPad* binPad, GstElement* mix)
{
	GstPad* muxerPad;
	gchar* binPadName, *muxerPadName;
	//mixerPad = gst_element_get_compatible_pad(mix, binPad, NULL);
	muxerPad = gst_element_get_request_pad(mix, "audio_%u");
	binPadName = gst_pad_get_name(binPad);
	muxerPadName = gst_pad_get_name(muxerPad);
	GstPadLinkReturn retVal = gst_pad_link(binPad, muxerPad);  // check if succesfull;
	g_print(" a new link is creatd with %s and %s pads\n", binPadName, muxerPadName);
	g_free(binPadName);
	g_free(muxerPadName);
	//gst_object_unref(binPad);
	//gst_object_unref(mixerPad);
	gst_element_release_request_pad(mix, muxerPad);
	return retVal;
}


GstPadLinkReturn video_link_to_muxer(GstPad* binPad, GstElement* mix)
{
	GstPad* muxerPad;
	gchar* binPadName, *muxerPadName;
	//mixerPad = gst_element_get_compatible_pad(mix, binPad, NULL);
	muxerPad = gst_element_get_request_pad(mix, "video_%u");
	binPadName = gst_pad_get_name(binPad);
	muxerPadName = gst_pad_get_name(muxerPad);
	GstPadLinkReturn retVal = gst_pad_link(binPad, muxerPad);  // check if succesfull;
	g_print(" a new link is creatd with %s and %s pads\n", binPadName, muxerPadName);
	g_free(binPadName);
	g_free(muxerPadName);
	//gst_object_unref(binPad);
	//gst_object_unref(mixerPad);
	gst_element_release_request_pad(mix, muxerPad);
	return retVal;
}

GstPadLinkReturn link_to_mpeg_muxer(GstPad* binPad, GstElement* mix)
{
	GstPad* muxerPad;
	gchar* binPadName, *muxerPadName;
	muxerPad = gst_element_get_compatible_pad(mix, binPad, NULL);
	//muxerPad = gst_element_get_request_pad(mix, "sink_%d");
	binPadName = gst_pad_get_name(binPad);
	muxerPadName = gst_pad_get_name(muxerPad);
	GstPadLinkReturn retVal = gst_pad_link(binPad, muxerPad);  // check if succesfull;
	g_print(" a new link is creatd with %s and %s pads\n", binPadName, muxerPadName);
	g_free(binPadName);
	g_free(muxerPadName);
	//gst_object_unref(binPad);
	gst_object_unref(muxerPad);
	//gst_element_release_request_pad(mix, muxerPad);
	return retVal;
}
GstPad* retrieve_ghost_pad(GstElement* bin, GstElement* elem)
{
	GstPad* elemPad = gst_element_get_static_pad(elem, "src");
	GstPad* ghost = gst_ghost_pad_new("ghostsrc", elemPad);
	gst_element_add_pad(bin, ghost);
	gst_object_unref(elemPad);
	return ghost;
}


static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
	GMainLoop *loop = (GMainLoop *)data;

	switch (GST_MESSAGE_TYPE(msg)) {

	case GST_MESSAGE_EOS:
	{
		g_print("End of stream\n");
		g_main_loop_quit(loop);
		break;

	}

	case GST_MESSAGE_ERROR: {
		gchar  *debug;
		GError *error;

		gst_message_parse_error(msg, &error, &debug);
		g_free(debug);

		g_printerr("Error: %s\n", error->message);
		g_error_free(error);

		g_main_loop_quit(loop);
		break;
	}
	case GST_MESSAGE_STATE_CHANGED:
	{

		GstState old_state, new_state;

		gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);
		g_print("Element %s changed state from %s to %s.\n",
			GST_OBJECT_NAME(msg->src),
			gst_element_state_get_name(old_state),
			gst_element_state_get_name(new_state));

		//if (new_state == GST_STATE_PAUSED)
		//{
		//	gst_element_set_state(mainPipeline, GST_STATE_NULL);
		//}

		break;

	}
	break;
	default:
		break;
	}

	return TRUE;
}

int main(int argc, char** argv)
{
	//gst - launch - 1.0.exe wasapisrc loopback = true\
	  // ! audiorate ! queue ! mix. wasapisrc low-latency=true \
  // ! audiorate ! queue ! mix. audiomixer name=mix ! queue ! audioconvert \
  // ! queue ! avenc_aac ! queue ! muxer. gdiscreencapsrc ! videoconvert \
  // ! x264enc ! mpegtsmux name = muxer !queue ! filesink location=muxin.mp4 sync=false
		
	elemStruct* mainStruct = new elemStruct();;
	if (!gst_init_check(&argc, &argv, NULL))
	{
		g_printerr("couldn't initialize gstreamer\n");
		return -1;
	}
	mainLoop = g_main_loop_new(NULL, FALSE);

	if ((mainPipeline = gst_pipeline_new("main_pipeline")) == NULL)
	{

	}
	mainStruct->micSource = gst_element_factory_make("wasapisrc", "mic_source");
	mainStruct->soundCardSrc = gst_element_factory_make("wasapisrc", "sound_card_source");
	mainStruct->gdiGrabber = gst_element_factory_make("dx9screencapsrc", "dx9_screen_capture_source");
	

	mainStruct->micSourceQueue = gst_element_factory_make("queue", "mic_source_queue_elem");
	mainStruct->soundCardSrcQueue = gst_element_factory_make("queue", "sound_card_source_queue");
	
	mainStruct->micSrcRate = gst_element_factory_make("audiorate", "mic_audio_rate_elem");
	mainStruct->soundCardRate = gst_element_factory_make("audiorate", "soundCard_audiorate_elem");
	
	mainStruct->micRateQueue = gst_element_factory_make("queue", "mic_audiorate_queue");
	mainStruct->soundCardRateQueue = gst_element_factory_make("queue", "soundCard_audiorate_queue");	

	mainStruct->audioMixer = gst_element_factory_make("audiomixer", "audio_mixer_elem");

	mainStruct->audioMixerQueue = gst_element_factory_make("queue", "audio_mixer_queue_elem");

	mainStruct->soundCardTempSink = gst_element_factory_make("autoaudiosink", "soundcard_temp_sink_elem");
	mainStruct->audioEncoder = gst_element_factory_make("avenc_aac", "audio_encoder_elem");
	mainStruct->audioEncoderQueue = gst_element_factory_make("queue", "audio_encoder_queue_elem");
	mainStruct->audioConverter = gst_element_factory_make("audioconvert", "audio_convert_elem");
	mainStruct->audioConverterQueue = gst_element_factory_make("queue", "audio_convert_queue_elem");
	mainStruct->gdiGrabberQueue = gst_element_factory_make("queue", "gdi_grabber_queue_elem");
	mainStruct->gdiGrabber = gst_element_factory_make("dx9screencapsrc", "gdi_grabber_elem");
	mainStruct->videoConverterQueue = gst_element_factory_make("queue", "videoconvert_queue_elem");
	mainStruct->x264encoderQueue = gst_element_factory_make("queue", "x264encoder_queue_elem");
	mainStruct->videoConverter = gst_element_factory_make("videoconvert", "videoconvert_elem");
	mainStruct->x264encoder = gst_element_factory_make("x264enc", "x264enc_elem");
	mainStruct->avMuxer = gst_element_factory_make("mpegtsmux", "mp4_muxer_elem");
	//if ((avMuxer = gst_element_factory_make("mpegtsmux", "mp4_muxer_elem")) == NULL)
	mainStruct->fileSinker = gst_element_factory_make("filesink", "filesink_elem");
	// set up all the sources
	g_object_set(G_OBJECT(mainStruct->micSource), "do-timestamp", true, NULL);
	g_object_set(G_OBJECT(mainStruct->soundCardSrc), "do-timestamp", true, "loopback", true, NULL);
	g_object_set(G_OBJECT(mainStruct->gdiGrabber), "do-timestamp", true, "cursor", true, NULL);
	g_object_set(G_OBJECT(mainStruct->x264encoder), "pass", 17, NULL);
	g_object_set(G_OBJECT(mainStruct->fileSinker), "location", "sani_1486.mp4", "sync", false, NULL);
	// set up all the queues
	set_queue_property(mainStruct->micSourceQueue);
	set_queue_property(mainStruct->soundCardSrcQueue);
	set_queue_property(mainStruct->audioMixerQueue);
	set_queue_property(mainStruct->audioEncoderQueue);
	set_queue_property(mainStruct->gdiGrabberQueue);
	set_queue_property(mainStruct->videoConverterQueue);
	set_queue_property(mainStruct->x264encoderQueue);


	// add the src elements to each src bin
	gst_bin_add_many(GST_BIN(mainPipeline), mainStruct->micSource, mainStruct->micSourceQueue, mainStruct->soundCardSrc, mainStruct->soundCardSrcQueue,  NULL);


	// link elements in each source bin
	gst_element_link(mainStruct->micSource, mainStruct->micSourceQueue);
	gst_element_link_many(mainStruct->soundCardSrc, mainStruct->soundCardSrcQueue, NULL);

	// put this two bin in audiobin, we will connect audiobin to screenBin later
	gst_bin_add_many(GST_BIN(mainPipeline),mainStruct->audioMixer, mainStruct->audioMixerQueue, mainStruct->audioEncoder, mainStruct->audioEncoderQueue, NULL);	

	//GstPad* soundSourceprober = gst_element_get_static_pad(mainStruct->soundCardSrc, "src");
	//gst_pad_add_probe(soundSourceprober, GST_PAD_PROBE_TYPE_BUFFER, soundCardProbe, &mainStruct, NULL);
	//GstStateChangeReturn ret = gst_element_set_state(mainStruct->soundCardSrc, GST_STATE_PLAYING);
	//GstStateChangeReturn retu = gst_element_get_state(mainStruct->soundCardSrc);



	mainStruct->micMixPad = gst_element_get_static_pad(mainStruct->micSourceQueue, "src");
	link_to_mixer(mainStruct->micMixPad, mainStruct->audioMixer);
	mainStruct->soundCardMixPad = gst_element_get_static_pad(mainStruct->soundCardSrcQueue, "src");
	link_to_mixer(mainStruct->soundCardMixPad, mainStruct->audioMixer);

	bool one_ = gst_element_link_many(mainStruct->audioMixer, mainStruct->audioMixerQueue, mainStruct->audioEncoder, mainStruct->audioEncoderQueue, NULL);

	gst_bin_add_many(GST_BIN(mainPipeline), mainStruct->gdiGrabber, mainStruct->gdiGrabberQueue, mainStruct->videoConverterQueue, mainStruct->videoConverter, mainStruct->x264encoder, mainStruct->x264encoderQueue, NULL);
	
	// so add this element , with main bin 
	gst_element_link_many(mainStruct->gdiGrabber, mainStruct->gdiGrabberQueue, mainStruct->videoConverter, mainStruct->videoConverterQueue, mainStruct->x264encoder, mainStruct->x264encoderQueue, NULL);

	//link_to_mixer(videoMuxPad, avMuxer);
	 mainStruct->videoMuxPad = gst_element_get_static_pad(mainStruct->x264encoderQueue, "src");
	 mainStruct->audioMuxPad = gst_element_get_static_pad(mainStruct->audioEncoderQueue, "src");

	// add all the bin and muxer and filesink to main pipeline bin 
	gst_bin_add_many(GST_BIN(mainPipeline), mainStruct->avMuxer, mainStruct->fileSinker, NULL);

	link_to_mpeg_muxer(mainStruct->videoMuxPad, mainStruct->avMuxer);
	link_to_mpeg_muxer(mainStruct->audioMuxPad, mainStruct->avMuxer);
	gst_element_link(mainStruct->avMuxer, mainStruct->fileSinker);
	
	//gst_element_link(videoMuxPad, avMuxer);


	/* Start playing the pipeline */
	mainStruct->ret = gst_element_set_state(mainPipeline, GST_STATE_PLAYING);
	// TODO , deal with ret

	mainStruct->mainBus = gst_element_get_bus(mainPipeline);
	mainStruct->bus_watch_id = gst_bus_add_watch(mainStruct->mainBus, bus_call, mainLoop);
	gst_object_unref(mainStruct->mainBus);
	// msg = gst_bus_timed_pop_filtered(mainBus, GST_CLOCK_TIME_NONE, GstMessageType(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
	g_main_loop_run(mainLoop);

	gst_element_set_state(mainPipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(mainPipeline));
	g_source_remove(mainStruct->bus_watch_id);
	g_main_loop_unref(mainLoop);
	//g_main_loop_quit(mainLoop);
	return 0;
}