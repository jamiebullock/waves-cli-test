/*
 ==============================================================================
 
 This file contains the basic startup code for a JUCE application.
 
 ==============================================================================
 */

#include <JuceHeader.h>

using namespace juce;

void testAudioPlugins(const String& pathToBundle, const String& pluginName, int numIterations, bool verbose)
{
   VST3PluginFormat format;
   AudioPluginFormatManager manager;
   
   KnownPluginList list;
   OwnedArray<PluginDescription> types;
   
   list.scanAndAddFile(pathToBundle, false, types, format);
   
   juce::PluginDescription desc;
   
   DBG("***** ENUMERATING TYPES");
   
   for ( auto& type : types)
   {
      DBG( "\tTYPE NAME: " + type->name );
      if (type->name == pluginName)
      {
         desc = *type;
         break;
      }
   }
   
   DBG("***** ITERATING CREATE / SET CYCLES");
   
   manager.addDefaultFormats();
   
   String errorMessage;
   
   for (int i = 0; i < numIterations; ++i)
   {
      
      DBG("==================== ITERATION " + String(i));
      
      std::unique_ptr<juce::AudioPluginInstance> instance = manager.createPluginInstance(desc, 48000, 2048, errorMessage);
      
      if (errorMessage.isNotEmpty())
      {
         DBG(errorMessage);
         jassertfalse;
      }
      
      const auto params = instance->getParameterTree().getParameters(true);
      
      for ( int j = 0; j < params.size(); ++j )
      {
         auto *param = params[j];
         auto value = param->getValue();
         
         if (verbose)
         {
            DBG("SETTING VALUE FOR " + instance->getName() + "." + param->getName(1024) + " TO " + String(value));
         }
         param->setValue(value);
      }
      
      DBG("\n");
   }
   
}

//==============================================================================
int main (int argc, char* argv[])
{
   
   ScopedJuceInitialiser_GUI init;
   
   const auto args = ArgumentList(argc, argv);
   
   String path = args.getValueForOption("--path");
   String pluginName = args.getValueForOption("--name");
   const String numIterationsOpt = args.getValueForOption("--iterations");
   const String verboseOpt = args.getValueForOption("--verbose");
   int numIterations = 1000;
   bool verbose = true;
   
   if ( path.isEmpty() )
   {
      path = "/Library/Audio/Plug-Ins/VST3/WaveShell1-VST3 13.0.vst3";
   }
   if (pluginName.isEmpty() )
   {
      pluginName = "Q10 Stereo";
   }
   if ( numIterationsOpt.isNotEmpty() )
   {
      numIterations = numIterationsOpt.getIntValue();
   }
   if ( verboseOpt.isNotEmpty() )
   {
      verbose = verboseOpt.getIntValue();
   }
   
   testAudioPlugins(path, pluginName, numIterations, verbose);
   
   return 0;
}
