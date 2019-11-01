//
//  lvglsample_main.cpp
//
//  Copyright (c) 2019 plan44.ch. All rights reserved.
//

#include "application.hpp"

#include "macaddress.hpp"
#include "utils.hpp"
#include "jsonobject.hpp"

#include <stdio.h>

#include "lvglui.hpp"

#define UICONFIG_FILE_NAME "uiconfig.json"

#define FATAL_ERROR_IMG "errorscreen.png"



using namespace p44;

class P44LvglSample : public CmdLineApp
{
  typedef CmdLineApp inherited;

  // app
  LvGLUi ui;
  MLTicket appTicket;

public:

  P44LvglSample()
  {
    ui.isMemberVariable();
  }

  virtual int main(int argc, char **argv)
  {
    const char *usageText =
    "Usage: %1$s [options]\n";
    const CmdLineOptionDescriptor options[] = {
      { 'u' , "uiconfig",     true, "filepath;path for UI definition file, defaults to " UICONFIG_FILE_NAME},
      #if MOUSE_CURSOR_SUPPORT
      { 0  , "mousecursor",     false, "show mouse cursor" },
      #endif
      CMDLINE_APPLICATION_PATHOPTIONS,
      DAEMON_APPLICATION_LOGOPTIONS,
      CMDLINE_APPLICATION_STDOPTIONS,
      { 0, NULL } // list terminator
    };

    // parse the command line, exits when syntax errors occur
    setCommandDescriptors(usageText, options);
    parseCommandLine(argc, argv);
    processStandardLogOptions(true); // daemon defaults

    if (numOptions()<1) {
      // show usage
      showUsage();
      terminateApp(EXIT_SUCCESS);
    }

    // app now ready to run
    return run();
  }



  // MARK: - initialisation

  virtual void initialize()
  {
    LOG(LOG_NOTICE, "P44LvglSample: initialize");
    // start littlevGL
    initLvgl();
  }


  // MARK: - script and config interface

  /// callback function for function evaluation
  /// @param aFunc the name of the function to execute, always passed in in all lowercase
  /// @param aArgs vector of function arguments, tuple contains expression starting position and value
  /// @param aResult set to function's result
  /// @return true if function executed, false if function signature (name, number of args) is unknown
  bool uiFunctionHandler(EvaluationContext* aEvalContext, const string& aFunc, const FunctionArguments& aArgs, ExpressionValue& aResult)
  {
    if (aFunc=="dosomething" && aArgs.size()==1) {
      // dosomething(something)
      LOG(LOG_NOTICE, "App should DO SOMETHING: %s", aArgs[0].stringValue().c_str())
    }
    else {
      // unknown function
      return false;
    }
    return true;
  }


  // MARK: - littlevGL

  void initLvgl()
  {
    LOG(LOG_NOTICE, "initializing littlevGL");
    LvGL::lvgl().init(getOption("mousecursor"));
    // real app UI
    // - install app specific script functions
    ui.uiScriptContext.registerFunctionHandler(boost::bind(&P44LvglSample::uiFunctionHandler, this, _1, _2, _3, _4));
    // - get config
    ErrorPtr err;
    string cfgPath = UICONFIG_FILE_NAME;
    getStringOption("uiconfig", cfgPath);
    JsonObjectPtr uiConfig = JsonObject::objFromFile(dataPath(cfgPath).c_str(), &err, true);
    if (Error::isError(err, SysError::domain(), ENOENT)) {
      // try resources
      uiConfig = JsonObject::objFromFile(resourcePath(cfgPath).c_str(), &err, true);
    }
    if (uiConfig && Error::isOK(err)) {
      err = ui.initForDisplay(lv_disp_get_default(), uiConfig);
    }
    if (Error::notOK(err)) {
      LOG(LOG_ERR, "Failed creating UI from config: %s", Error::text(err));
      fatalErrorScreen(string_format("UI config error: %s", Error::text(err)));
    }
  }


  void fatalErrorScreen(const string aMessage)
  {
    lv_obj_t* errorScreen = lv_obj_create(NULL, NULL);
    // error label
    lv_obj_t* errLabel = lv_label_create(errorScreen, NULL);
    static lv_style_t errLabelStyle;
    lv_style_copy(&errLabelStyle, &lv_style_plain);
    errLabelStyle.text.font = &lv_font_roboto_16;
    lv_obj_set_style(errLabel, &errLabelStyle);
    lv_label_set_long_mode(errLabel, LV_LABEL_LONG_SROLL_CIRC);
    lv_label_set_align(errLabel, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(errLabel, aMessage.c_str());
    lv_obj_set_width(errLabel, lv_obj_get_width(errorScreen)-10); // expand to full width
    lv_obj_set_height(errLabel, 42);
    lv_obj_align(errLabel, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    // activate the error screen
    lv_scr_load(errorScreen);
  }

};


int main(int argc, char **argv)
{
  // prevent all logging until command line determines level
  SETLOGLEVEL(LOG_EMERG);
  SETERRLEVEL(LOG_EMERG, false); // messages, if any, go to stderr

  // create app with current mainloop
  P44LvglSample *application = new(P44LvglSample);
  // pass control
  int status = application->main(argc, argv);
  // done
  delete application;
  return status;
}

