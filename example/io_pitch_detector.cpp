
#include <q/support/literals.hpp>
#include <q/support/decibel.hpp>
#include <q/pitch/pitch_detector.hpp>
#include <q_io/audio_stream.hpp>
#include "example.hpp"
#include <q/utility/sleep.hpp>


namespace q = cycfi::q;
using namespace q::literals;

struct pitch_processor : q::audio_stream
{
   pitch_processor(
      int device_id
    , q::decibel hysteresis
    , q::frequency min
    , q::frequency max
   )
    : audio_stream(q::audio_device::get(device_id), 1, 0)
    , _pd(min, max, sampling_rate(), hysteresis)
   {}

   void process(in_channels const& in) override
   {
      auto ch0 = in[0];
      for (auto frame: in.frames)
      {
         auto s = ch0[frame];

         bool is_ready = _pd(s);

         if (is_ready)
         {
            auto frequency = _pd.get_frequency();
            std::cout << "Frequency: " << frequency << " Hz" << std::endl;
         }
      }
   }

   q::pitch_detector _pd;
};

int main()
{
   std::cout << "Please set pitch detector hysteresis in decibels (e.g. -45)" << std::endl;
   float h;
   std::cin >> h;
   q::decibel hysteresis{ h, q::direct_unit };

   std::cout << "Please set minimum frequency in Hz:" << std::endl;
   double min_freq;
   std::cin >> min_freq;
   q::frequency min_frequency{ min_freq, q::direct_unit };

   std::cout << "Please set maximum frequency in Hz:" << std::endl;
   double max_freq;
   std::cin >> max_freq;
   q::frequency max_frequency{ max_freq, q::direct_unit };

   auto audio_device_id = get_audio_device();
   auto proc = pitch_processor{ audio_device_id, hysteresis, min_frequency, max_frequency };

   if (proc.is_valid())
   {
      proc.start();
      while (running)
         q::sleep(1_s);
      proc.stop();
   }
   else
   {
      std::cout << "Invalid sound processor -- try with a different audio device" << std::endl;
   }

   return 0;
}