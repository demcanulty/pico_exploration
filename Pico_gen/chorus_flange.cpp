#include "chorus_flange.h"

namespace chorus_flange {

/****************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER 200k USD in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below 200k USD annual revenue or funding.

For entities with OVER 200k USD in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing (at) cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/360050779193-Gen-Code-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
****************************************************************************************/

// global noise generator
Noise noise;
static const int GENLIB_LOOPCOUNT_BAIL = 100000;


// The State struct contains all the state and procedures for the gendsp kernel
typedef struct State {
	CommonState __commonstate;
	Data m_tospeakers_9;
	Data m_speaker_loc_23;
	Data m_amplitudes_10;
	Delay m_delay_1;
	Delay m_delay_7;
	Delay m_delay_3;
	Delay m_delay_5;
	SineCycle __m_cycle_117;
	SineCycle __m_cycle_71;
	SineCycle __m_cycle_48;
	SineCycle __m_cycle_94;
	SineData __sinedata;
	int __loopcount;
	int __exception;
	int vectorsize;
	t_sample m_prev_74;
	t_sample m_prev_28;
	t_sample m_prev_26;
	t_sample m_prev_72;
	t_sample m_prev_97;
	t_sample m_prev_49;
	t_sample m_prev_95;
	t_sample m_prev_51;
	t_sample m_loadbang_22;
	t_sample m_rot_dry_18;
	t_sample m_dry_loc_20;
	t_sample m_history_6;
	t_sample m_history_8;
	t_sample m_wet_loc_11;
	t_sample m_history_4;
	t_sample samplerate;
	t_sample m_history_2;
	t_sample m_wet_21;
	t_sample m_rot_wet_12;
	t_sample m_delGain_14;
	t_sample m_prev_118;
	t_sample m_dry_19;
	t_sample m_fb_13;
	t_sample m_spread_17;
	t_sample m_rate_15;
	t_sample m_time_16;
	t_sample m_prev_120;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_delay_1.reset("m_delay_1", ((int)4096));
		m_history_2 = ((int)0);
		m_delay_3.reset("m_delay_3", ((int)4096));
		m_history_4 = ((int)0);
		m_delay_5.reset("m_delay_5", ((int)4096));
		m_history_6 = ((int)0);
		m_delay_7.reset("m_delay_7", ((int)4096));
		m_history_8 = ((int)0);
		m_tospeakers_9.reset("tospeakers", ((int)4), ((int)1));
		m_amplitudes_10.reset("amplitudes", ((int)4), ((int)1));
		m_wet_loc_11 = ((t_sample)0.7);
		m_rot_wet_12 = 0;
		m_fb_13 = ((t_sample)0.3);
		m_delGain_14 = ((int)1);
		m_rate_15 = ((t_sample)0.2);
		m_time_16 = ((int)5);
		m_spread_17 = ((t_sample)0.01);
		m_rot_dry_18 = 0;
		m_dry_19 = ((int)1);
		m_dry_loc_20 = ((t_sample)0.7);
		m_wet_21 = ((t_sample)0.5);
		m_loadbang_22 = ((int)1);
		m_speaker_loc_23.reset("speaker_loc", ((int)17), ((int)1));
		m_prev_26 = ((int)0);
		m_prev_28 = ((int)0);
		__m_cycle_48.reset(samplerate, 0);
		m_prev_49 = ((int)0);
		m_prev_51 = ((int)0);
		__m_cycle_71.reset(samplerate, 0);
		m_prev_72 = ((int)0);
		m_prev_74 = ((int)0);
		__m_cycle_94.reset(samplerate, 0);
		m_prev_95 = ((int)0);
		m_prev_97 = ((int)0);
		__m_cycle_117.reset(samplerate, 0);
		m_prev_118 = ((int)0);
		m_prev_120 = ((int)0);
		genlib_reset_complete(this);
		
	};
	// the signal processing routine;
	inline int perform(t_sample ** __ins, t_sample ** __outs, int __n) {
		vectorsize = __n;
		const t_sample * __in1 = __ins[0];
		t_sample * __out1 = __outs[0];
		t_sample * __out2 = __outs[1];
		t_sample * __out3 = __outs[2];
		t_sample * __out4 = __outs[3];
		if (__exception) {
			return __exception;
			
		} else if (( (__in1 == 0) || (__out1 == 0) || (__out2 == 0) || (__out3 == 0) || (__out4 == 0) )) {
			__exception = GENLIB_ERR_NULL_BUFFER;
			return __exception;
			
		};
		__loopcount = (__n * GENLIB_LOOPCOUNT_BAIL);
		int amplitudes_dim = m_amplitudes_10.dim;
		int amplitudes_channels = m_amplitudes_10.channels;
		t_sample mul_313 = (m_rate_15 * m_spread_17);
		t_sample add_312 = (m_rate_15 + mul_313);
		t_sample mul_315 = (m_time_16 * m_spread_17);
		t_sample add_314 = (m_time_16 + mul_315);
		t_sample mstosamps_327 = (add_314 * (samplerate * 0.001));
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			if (m_loadbang_22) {
				// for loop initializer;
				int i = ((int)1);
				// for loop condition;
				while ((i <= ((int)16))) {
					// abort processing if an infinite loop is suspected;
					if (((__loopcount--) <= 0)) {
						__exception = GENLIB_ERR_LOOP_OVERFLOW;
						break ;
						
					};
					int speaker_loc_dim = m_speaker_loc_23.dim;
					int speaker_loc_channels = m_speaker_loc_23.channels;
					bool index_ignore_24 = ((i >= speaker_loc_dim) || (i < 0));
					if ((!index_ignore_24)) {
						int choice_25 = i;
						m_speaker_loc_23.write(((choice_25 >= 16) ? ((int)0) : ((choice_25 >= 15) ? ((int)-1) : ((choice_25 >= 14) ? ((int)-1) : ((choice_25 >= 13) ? ((int)0) : ((choice_25 >= 12) ? ((int)0) : ((choice_25 >= 11) ? ((int)1) : ((choice_25 >= 10) ? ((int)1) : ((choice_25 >= 9) ? ((int)0) : ((choice_25 >= 8) ? ((t_sample)-0.707) : ((choice_25 >= 7) ? ((t_sample)-0.707) : ((choice_25 >= 6) ? ((t_sample)-0.707) : ((choice_25 >= 5) ? ((t_sample)0.707) : ((choice_25 >= 4) ? ((t_sample)0.707) : ((choice_25 >= 3) ? ((t_sample)0.707) : ((choice_25 >= 2) ? ((t_sample)0.707) : ((choice_25 >= 1) ? ((t_sample)-0.707) : 0)))))))))))))))), i, 0);
						
					};
					// for loop increment;
					i = (i + ((int)1));
					
				};
				m_loadbang_22 = ((int)0);
				
			};
			// abort processing if an infinite loop is suspected;
			if (((__loopcount--) <= 0)) {
				__exception = GENLIB_ERR_LOOP_OVERFLOW;
				break ;
				
			};
			t_sample y_loc = ((m_dry_loc_20 <= ((int)-1)) ? ((int)-1) : ((m_dry_loc_20 >= ((int)1)) ? ((int)1) : m_dry_loc_20));
			t_sample atten = ((m_dry_19 <= ((int)0)) ? ((int)0) : ((m_dry_19 >= ((int)1)) ? ((int)1) : m_dry_19));
			t_sample rotate = ((m_rot_dry_18 <= ((int)0)) ? ((int)0) : ((m_rot_dry_18 >= ((int)1)) ? ((int)1) : m_rot_dry_18));
			t_sample x_loc = ((m_dry_loc_20 <= ((int)-1)) ? ((int)-1) : ((m_dry_loc_20 >= ((int)1)) ? ((int)1) : m_dry_loc_20));
			t_sample cartopol_613 = sqrt(((x_loc * x_loc) + (y_loc * y_loc)));
			t_sample cartopol_614 = atan2(y_loc, x_loc);
			t_sample dist_358 = ((cartopol_613 <= ((int)0)) ? ((int)0) : ((cartopol_613 >= ((int)1)) ? ((int)1) : cartopol_613));
			t_sample angle_rot = (cartopol_614 + (((t_sample)6.2831853071796) * rotate));
			t_sample poltocar_615 = (dist_358 * cos(angle_rot));
			t_sample poltocar_616 = (dist_358 * sin(angle_rot));
			t_sample x_rad_sm = smooth_d(poltocar_615);
			t_sample y_rad_sm = smooth_d_27(poltocar_616);
			// for loop initializer;
			int i_397 = ((int)0);
			// for loop condition;
			while ((i_397 < ((int)4))) {
				// abort processing if an infinite loop is suspected;
				if (((__loopcount--) <= 0)) {
					__exception = GENLIB_ERR_LOOP_OVERFLOW;
					break ;
					
				};
				int si = (((int)1) + (i_397 * ((int)2)));
				int speaker_loc_dim = m_speaker_loc_23.dim;
				int speaker_loc_channels = m_speaker_loc_23.channels;
				bool index_ignore_29 = ((si >= speaker_loc_dim) || (si < 0));
				// samples speaker_loc channel 1;
				int chan_30 = ((int)0);
				bool chan_ignore_31 = ((chan_30 < 0) || (chan_30 >= speaker_loc_channels));
				t_sample x_spk = ((chan_ignore_31 || index_ignore_29) ? 0 : m_speaker_loc_23.read(si, chan_30));
				bool index_ignore_32 = (((si + ((int)1)) >= speaker_loc_dim) || ((si + ((int)1)) < 0));
				// samples speaker_loc channel 1;
				int chan_33 = ((int)0);
				bool chan_ignore_34 = ((chan_33 < 0) || (chan_33 >= speaker_loc_channels));
				t_sample y_spk = ((chan_ignore_34 || index_ignore_32) ? 0 : m_speaker_loc_23.read((si + ((int)1)), chan_33));
				t_sample dx = (x_rad_sm - x_spk);
				t_sample dy = (y_rad_sm - y_spk);
				t_sample v_35 = sqrt(((dx * dx) + (dy * dy)));
				t_sample distance = ((v_35 <= ((int)0)) ? ((int)0) : ((v_35 >= ((t_sample)1.414)) ? ((t_sample)1.414) : v_35));
				t_sample sub_620 = (distance - ((int)0));
				t_sample scale_617 = ((safepow((sub_620 * ((t_sample)0.70721357850071)), ((int)1)) * ((t_sample)-1.5707963267949)) + ((t_sample)1.5707963267949));
				t_sample amp = (atten * sin(scale_617));
				int amplitudes_dim = m_amplitudes_10.dim;
				int amplitudes_channels = m_amplitudes_10.channels;
				bool index_ignore_36 = ((i_397 >= amplitudes_dim) || (i_397 < 0));
				if ((!index_ignore_36)) {
					m_amplitudes_10.write(amp, i_397, 0);
					
				};
				// for loop increment;
				i_397 = (i_397 + ((int)1));
				
			};
			// samples amplitudes channel 1;
			int chan_37 = ((int)0);
			bool chan_ignore_38 = ((chan_37 < 0) || (chan_37 >= amplitudes_channels));
			t_sample sample_amplitudes_355 = (chan_ignore_38 ? 0 : m_amplitudes_10.read(0, chan_37));
			t_sample index_amplitudes_356 = ((int)0);
			t_sample mul_353 = (in1 * sample_amplitudes_355);
			bool index_ignore_39 = (((int)1) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_40 = ((int)0);
			bool chan_ignore_41 = ((chan_40 < 0) || (chan_40 >= amplitudes_channels));
			t_sample sample_amplitudes_351 = ((chan_ignore_41 || index_ignore_39) ? 0 : m_amplitudes_10.read(((int)1), chan_40));
			t_sample index_amplitudes_352 = ((int)1);
			t_sample mul_349 = (in1 * sample_amplitudes_351);
			bool index_ignore_42 = (((int)3) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_43 = ((int)0);
			bool chan_ignore_44 = ((chan_43 < 0) || (chan_43 >= amplitudes_channels));
			t_sample sample_amplitudes_343 = ((chan_ignore_44 || index_ignore_42) ? 0 : m_amplitudes_10.read(((int)3), chan_43));
			t_sample index_amplitudes_344 = ((int)3);
			t_sample mul_341 = (in1 * sample_amplitudes_343);
			bool index_ignore_45 = (((int)2) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_46 = ((int)0);
			bool chan_ignore_47 = ((chan_46 < 0) || (chan_46 >= amplitudes_channels));
			t_sample sample_amplitudes_347 = ((chan_ignore_47 || index_ignore_45) ? 0 : m_amplitudes_10.read(((int)2), chan_46));
			t_sample index_amplitudes_348 = ((int)2);
			t_sample mul_345 = (in1 * sample_amplitudes_347);
			t_sample mul_251 = (m_wet_loc_11 * (-1));
			__m_cycle_48.freq(add_312);
			t_sample cycle_318 = __m_cycle_48(__sinedata);
			t_sample cycleindex_319 = __m_cycle_48.phase();
			t_sample mix_621 = (m_history_8 + (((t_sample)0.002617990887418) * (mstosamps_327 - m_history_8)));
			t_sample mix_320 = mix_621;
			t_sample gen_326 = mix_320;
			t_sample history_322_next_325 = fixdenorm(mix_320);
			t_sample mul_317 = (cycle_318 * gen_326);
			t_sample mul_311 = (gen_326 * ((t_sample)0.1));
			t_sample add_310 = (gen_326 + mul_311);
			t_sample add_316 = (mul_317 + add_310);
			t_sample tap_366 = m_delay_7.read_cubic(add_316);
			t_sample mul_363 = (tap_366 * m_delGain_14);
			t_sample y_loc_415 = ((mul_251 <= ((int)-1)) ? ((int)-1) : ((mul_251 >= ((int)1)) ? ((int)1) : mul_251));
			t_sample x_loc_414 = ((m_wet_loc_11 <= ((int)-1)) ? ((int)-1) : ((m_wet_loc_11 >= ((int)1)) ? ((int)1) : m_wet_loc_11));
			t_sample rotate_427 = ((m_rot_wet_12 <= ((int)0)) ? ((int)0) : ((m_rot_wet_12 >= ((int)1)) ? ((int)1) : m_rot_wet_12));
			t_sample atten_407 = ((m_wet_21 <= ((int)0)) ? ((int)0) : ((m_wet_21 >= ((int)1)) ? ((int)1) : m_wet_21));
			t_sample cartopol_622 = sqrt(((x_loc_414 * x_loc_414) + (y_loc_415 * y_loc_415)));
			t_sample cartopol_623 = atan2(y_loc_415, x_loc_414);
			t_sample dist_412 = ((cartopol_622 <= ((int)0)) ? ((int)0) : ((cartopol_622 >= ((int)1)) ? ((int)1) : cartopol_622));
			t_sample angle_rot_428 = (cartopol_623 + (((t_sample)6.2831853071796) * rotate_427));
			t_sample poltocar_624 = (dist_412 * cos(angle_rot_428));
			t_sample poltocar_625 = (dist_412 * sin(angle_rot_428));
			t_sample x_rad_sm_416 = smooth_398_d(poltocar_624);
			t_sample y_rad_sm_417 = smooth_398_d_50(poltocar_625);
			// for loop initializer;
			int i_434 = ((int)0);
			// for loop condition;
			while ((i_434 < ((int)4))) {
				// abort processing if an infinite loop is suspected;
				if (((__loopcount--) <= 0)) {
					__exception = GENLIB_ERR_LOOP_OVERFLOW;
					break ;
					
				};
				int si_423 = (((int)1) + (i_434 * ((int)2)));
				int speaker_loc_dim = m_speaker_loc_23.dim;
				int speaker_loc_channels = m_speaker_loc_23.channels;
				bool index_ignore_52 = ((si_423 >= speaker_loc_dim) || (si_423 < 0));
				// samples speaker_loc channel 1;
				int chan_53 = ((int)0);
				bool chan_ignore_54 = ((chan_53 < 0) || (chan_53 >= speaker_loc_channels));
				t_sample x_spk_426 = ((chan_ignore_54 || index_ignore_52) ? 0 : m_speaker_loc_23.read(si_423, chan_53));
				bool index_ignore_55 = (((si_423 + ((int)1)) >= speaker_loc_dim) || ((si_423 + ((int)1)) < 0));
				// samples speaker_loc channel 1;
				int chan_56 = ((int)0);
				bool chan_ignore_57 = ((chan_56 < 0) || (chan_56 >= speaker_loc_channels));
				t_sample y_spk_425 = ((chan_ignore_57 || index_ignore_55) ? 0 : m_speaker_loc_23.read((si_423 + ((int)1)), chan_56));
				t_sample dx_437 = (x_rad_sm_416 - x_spk_426);
				t_sample dy_420 = (y_rad_sm_417 - y_spk_425);
				t_sample v_58 = sqrt(((dx_437 * dx_437) + (dy_420 * dy_420)));
				t_sample distance_435 = ((v_58 <= ((int)0)) ? ((int)0) : ((v_58 >= ((t_sample)1.414)) ? ((t_sample)1.414) : v_58));
				t_sample sub_629 = (distance_435 - ((int)0));
				t_sample scale_626 = ((safepow((sub_629 * ((t_sample)0.70721357850071)), ((int)1)) * ((t_sample)-1.5707963267949)) + ((t_sample)1.5707963267949));
				t_sample amp_419 = (atten_407 * sin(scale_626));
				bool index_ignore_59 = ((i_434 >= amplitudes_dim) || (i_434 < 0));
				if ((!index_ignore_59)) {
					m_amplitudes_10.write(amp_419, i_434, 0);
					
				};
				// for loop increment;
				i_434 = (i_434 + ((int)1));
				
			};
			// samples amplitudes channel 1;
			int chan_60 = ((int)0);
			bool chan_ignore_61 = ((chan_60 < 0) || (chan_60 >= amplitudes_channels));
			t_sample sample_amplitudes_403 = (chan_ignore_61 ? 0 : m_amplitudes_10.read(0, chan_60));
			t_sample index_amplitudes_410 = ((int)0);
			t_sample mul_424 = (mul_363 * sample_amplitudes_403);
			bool index_ignore_62 = (((int)1) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_63 = ((int)0);
			bool chan_ignore_64 = ((chan_63 < 0) || (chan_63 >= amplitudes_channels));
			t_sample sample_amplitudes_405 = ((chan_ignore_64 || index_ignore_62) ? 0 : m_amplitudes_10.read(((int)1), chan_63));
			t_sample index_amplitudes_418 = ((int)1);
			t_sample mul_404 = (mul_363 * sample_amplitudes_405);
			bool index_ignore_65 = (((int)3) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_66 = ((int)0);
			bool chan_ignore_67 = ((chan_66 < 0) || (chan_66 >= amplitudes_channels));
			t_sample sample_amplitudes_402 = ((chan_ignore_67 || index_ignore_65) ? 0 : m_amplitudes_10.read(((int)3), chan_66));
			t_sample index_amplitudes_406 = ((int)3);
			t_sample mul_430 = (mul_363 * sample_amplitudes_402);
			bool index_ignore_68 = (((int)2) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_69 = ((int)0);
			bool chan_ignore_70 = ((chan_69 < 0) || (chan_69 >= amplitudes_channels));
			t_sample sample_amplitudes_411 = ((chan_ignore_70 || index_ignore_68) ? 0 : m_amplitudes_10.read(((int)2), chan_69));
			t_sample index_amplitudes_432 = ((int)2);
			t_sample mul_422 = (mul_363 * sample_amplitudes_411);
			t_sample mul_364 = (tap_366 * m_fb_13);
			t_sample mul_294 = (m_rate_15 * ((int)0));
			t_sample add_293 = (m_rate_15 + mul_294);
			__m_cycle_71.freq(add_293);
			t_sample cycle_299 = __m_cycle_71(__sinedata);
			t_sample cycleindex_300 = __m_cycle_71.phase();
			t_sample mul_296 = (m_time_16 * ((int)0));
			t_sample add_295 = (m_time_16 + mul_296);
			t_sample mstosamps_308 = (add_295 * (samplerate * 0.001));
			t_sample mix_630 = (m_history_6 + (((t_sample)0.002617990887418) * (mstosamps_308 - m_history_6)));
			t_sample mix_301 = mix_630;
			t_sample gen_307 = mix_301;
			t_sample history_303_next_306 = fixdenorm(mix_301);
			t_sample mul_298 = (cycle_299 * gen_307);
			t_sample mul_292 = (gen_307 * ((t_sample)0.1));
			t_sample add_291 = (gen_307 + mul_292);
			t_sample add_297 = (mul_298 + add_291);
			t_sample tap_396 = m_delay_5.read_cubic(add_297);
			t_sample mul_393 = (tap_396 * m_delGain_14);
			t_sample y_loc_470 = ((m_wet_loc_11 <= ((int)-1)) ? ((int)-1) : ((m_wet_loc_11 >= ((int)1)) ? ((int)1) : m_wet_loc_11));
			t_sample x_loc_469 = ((m_wet_loc_11 <= ((int)-1)) ? ((int)-1) : ((m_wet_loc_11 >= ((int)1)) ? ((int)1) : m_wet_loc_11));
			t_sample rotate_475 = ((m_rot_wet_12 <= ((int)0)) ? ((int)0) : ((m_rot_wet_12 >= ((int)1)) ? ((int)1) : m_rot_wet_12));
			t_sample atten_467 = ((m_wet_21 <= ((int)0)) ? ((int)0) : ((m_wet_21 >= ((int)1)) ? ((int)1) : m_wet_21));
			t_sample cartopol_631 = sqrt(((x_loc_469 * x_loc_469) + (y_loc_470 * y_loc_470)));
			t_sample cartopol_632 = atan2(y_loc_470, x_loc_469);
			t_sample dist_448 = ((cartopol_631 <= ((int)0)) ? ((int)0) : ((cartopol_631 >= ((int)1)) ? ((int)1) : cartopol_631));
			t_sample angle_rot_477 = (cartopol_632 + (((t_sample)6.2831853071796) * rotate_475));
			t_sample poltocar_633 = (dist_448 * cos(angle_rot_477));
			t_sample poltocar_634 = (dist_448 * sin(angle_rot_477));
			t_sample x_rad_sm_471 = smooth_440_d(poltocar_633);
			t_sample y_rad_sm_472 = smooth_440_d_73(poltocar_634);
			// for loop initializer;
			int i_465 = ((int)0);
			// for loop condition;
			while ((i_465 < ((int)4))) {
				// abort processing if an infinite loop is suspected;
				if (((__loopcount--) <= 0)) {
					__exception = GENLIB_ERR_LOOP_OVERFLOW;
					break ;
					
				};
				int si_454 = (((int)1) + (i_465 * ((int)2)));
				int speaker_loc_dim = m_speaker_loc_23.dim;
				int speaker_loc_channels = m_speaker_loc_23.channels;
				bool index_ignore_75 = ((si_454 >= speaker_loc_dim) || (si_454 < 0));
				// samples speaker_loc channel 1;
				int chan_76 = ((int)0);
				bool chan_ignore_77 = ((chan_76 < 0) || (chan_76 >= speaker_loc_channels));
				t_sample x_spk_456 = ((chan_ignore_77 || index_ignore_75) ? 0 : m_speaker_loc_23.read(si_454, chan_76));
				bool index_ignore_78 = (((si_454 + ((int)1)) >= speaker_loc_dim) || ((si_454 + ((int)1)) < 0));
				// samples speaker_loc channel 1;
				int chan_79 = ((int)0);
				bool chan_ignore_80 = ((chan_79 < 0) || (chan_79 >= speaker_loc_channels));
				t_sample y_spk_481 = ((chan_ignore_80 || index_ignore_78) ? 0 : m_speaker_loc_23.read((si_454 + ((int)1)), chan_79));
				t_sample dx_457 = (x_rad_sm_471 - x_spk_456);
				t_sample dy_452 = (y_rad_sm_472 - y_spk_481);
				t_sample v_81 = sqrt(((dx_457 * dx_457) + (dy_452 * dy_452)));
				t_sample distance_460 = ((v_81 <= ((int)0)) ? ((int)0) : ((v_81 >= ((t_sample)1.414)) ? ((t_sample)1.414) : v_81));
				t_sample sub_638 = (distance_460 - ((int)0));
				t_sample scale_635 = ((safepow((sub_638 * ((t_sample)0.70721357850071)), ((int)1)) * ((t_sample)-1.5707963267949)) + ((t_sample)1.5707963267949));
				t_sample amp_451 = (atten_467 * sin(scale_635));
				bool index_ignore_82 = ((i_465 >= amplitudes_dim) || (i_465 < 0));
				if ((!index_ignore_82)) {
					m_amplitudes_10.write(amp_451, i_465, 0);
					
				};
				// for loop increment;
				i_465 = (i_465 + ((int)1));
				
			};
			// samples amplitudes channel 1;
			int chan_83 = ((int)0);
			bool chan_ignore_84 = ((chan_83 < 0) || (chan_83 >= amplitudes_channels));
			t_sample sample_amplitudes_466 = (chan_ignore_84 ? 0 : m_amplitudes_10.read(0, chan_83));
			t_sample index_amplitudes_446 = ((int)0);
			t_sample mul_455 = (mul_393 * sample_amplitudes_466);
			bool index_ignore_85 = (((int)1) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_86 = ((int)0);
			bool chan_ignore_87 = ((chan_86 < 0) || (chan_86 >= amplitudes_channels));
			t_sample sample_amplitudes_443 = ((chan_ignore_87 || index_ignore_85) ? 0 : m_amplitudes_10.read(((int)1), chan_86));
			t_sample index_amplitudes_473 = ((int)1);
			t_sample mul_478 = (mul_393 * sample_amplitudes_443);
			bool index_ignore_88 = (((int)3) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_89 = ((int)0);
			bool chan_ignore_90 = ((chan_89 < 0) || (chan_89 >= amplitudes_channels));
			t_sample sample_amplitudes_442 = ((chan_ignore_90 || index_ignore_88) ? 0 : m_amplitudes_10.read(((int)3), chan_89));
			t_sample index_amplitudes_444 = ((int)3);
			t_sample mul_458 = (mul_393 * sample_amplitudes_442);
			bool index_ignore_91 = (((int)2) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_92 = ((int)0);
			bool chan_ignore_93 = ((chan_92 < 0) || (chan_92 >= amplitudes_channels));
			t_sample sample_amplitudes_447 = ((chan_ignore_93 || index_ignore_91) ? 0 : m_amplitudes_10.read(((int)2), chan_92));
			t_sample index_amplitudes_459 = ((int)2);
			t_sample mul_453 = (mul_393 * sample_amplitudes_447);
			t_sample mul_394 = (tap_396 * m_fb_13);
			t_sample mul_250 = (m_spread_17 * ((int)2));
			t_sample mul_274 = (m_rate_15 * mul_250);
			t_sample add_273 = (m_rate_15 + mul_274);
			__m_cycle_94.freq(add_273);
			t_sample cycle_279 = __m_cycle_94(__sinedata);
			t_sample cycleindex_280 = __m_cycle_94.phase();
			t_sample mul_276 = (m_time_16 * mul_250);
			t_sample add_275 = (m_time_16 + mul_276);
			t_sample mstosamps_288 = (add_275 * (samplerate * 0.001));
			t_sample mix_639 = (m_history_4 + (((t_sample)0.002617990887418) * (mstosamps_288 - m_history_4)));
			t_sample mix_281 = mix_639;
			t_sample gen_287 = mix_281;
			t_sample history_283_next_286 = fixdenorm(mix_281);
			t_sample mul_278 = (cycle_279 * gen_287);
			t_sample mul_272 = (gen_287 * ((t_sample)0.1));
			t_sample add_271 = (gen_287 + mul_272);
			t_sample add_277 = (mul_278 + add_271);
			t_sample tap_336 = m_delay_3.read_cubic(add_277);
			t_sample mul_333 = (tap_336 * m_delGain_14);
			t_sample x_loc_512 = ((mul_251 <= ((int)-1)) ? ((int)-1) : ((mul_251 >= ((int)1)) ? ((int)1) : mul_251));
			t_sample atten_510 = ((m_wet_21 <= ((int)0)) ? ((int)0) : ((m_wet_21 >= ((int)1)) ? ((int)1) : m_wet_21));
			t_sample rotate_518 = ((m_rot_wet_12 <= ((int)0)) ? ((int)0) : ((m_rot_wet_12 >= ((int)1)) ? ((int)1) : m_rot_wet_12));
			t_sample cartopol_640 = sqrt(((x_loc_512 * x_loc_512) + ((int)0)));
			t_sample cartopol_641 = atan2(((int)0), x_loc_512);
			t_sample dist_490 = ((cartopol_640 <= ((int)0)) ? ((int)0) : ((cartopol_640 >= ((int)1)) ? ((int)1) : cartopol_640));
			t_sample angle_rot_519 = (cartopol_641 + (((t_sample)6.2831853071796) * rotate_518));
			t_sample poltocar_642 = (dist_490 * cos(angle_rot_519));
			t_sample poltocar_643 = (dist_490 * sin(angle_rot_519));
			t_sample x_rad_sm_514 = smooth_482_d(poltocar_642);
			t_sample y_rad_sm_515 = smooth_482_d_96(poltocar_643);
			// for loop initializer;
			int i_508 = ((int)0);
			// for loop condition;
			while ((i_508 < ((int)4))) {
				// abort processing if an infinite loop is suspected;
				if (((__loopcount--) <= 0)) {
					__exception = GENLIB_ERR_LOOP_OVERFLOW;
					break ;
					
				};
				int si_496 = (((int)1) + (i_508 * ((int)2)));
				int speaker_loc_dim = m_speaker_loc_23.dim;
				int speaker_loc_channels = m_speaker_loc_23.channels;
				bool index_ignore_98 = ((si_496 >= speaker_loc_dim) || (si_496 < 0));
				// samples speaker_loc channel 1;
				int chan_99 = ((int)0);
				bool chan_ignore_100 = ((chan_99 < 0) || (chan_99 >= speaker_loc_channels));
				t_sample x_spk_499 = ((chan_ignore_100 || index_ignore_98) ? 0 : m_speaker_loc_23.read(si_496, chan_99));
				bool index_ignore_101 = (((si_496 + ((int)1)) >= speaker_loc_dim) || ((si_496 + ((int)1)) < 0));
				// samples speaker_loc channel 1;
				int chan_102 = ((int)0);
				bool chan_ignore_103 = ((chan_102 < 0) || (chan_102 >= speaker_loc_channels));
				t_sample y_spk_498 = ((chan_ignore_103 || index_ignore_101) ? 0 : m_speaker_loc_23.read((si_496 + ((int)1)), chan_102));
				t_sample dx_500 = (x_rad_sm_514 - x_spk_499);
				t_sample dy_494 = (y_rad_sm_515 - y_spk_498);
				t_sample v_104 = sqrt(((dx_500 * dx_500) + (dy_494 * dy_494)));
				t_sample distance_503 = ((v_104 <= ((int)0)) ? ((int)0) : ((v_104 >= ((t_sample)1.414)) ? ((t_sample)1.414) : v_104));
				t_sample sub_647 = (distance_503 - ((int)0));
				t_sample scale_644 = ((safepow((sub_647 * ((t_sample)0.70721357850071)), ((int)1)) * ((t_sample)-1.5707963267949)) + ((t_sample)1.5707963267949));
				t_sample amp_493 = (atten_510 * sin(scale_644));
				bool index_ignore_105 = ((i_508 >= amplitudes_dim) || (i_508 < 0));
				if ((!index_ignore_105)) {
					m_amplitudes_10.write(amp_493, i_508, 0);
					
				};
				// for loop increment;
				i_508 = (i_508 + ((int)1));
				
			};
			// samples amplitudes channel 1;
			int chan_106 = ((int)0);
			bool chan_ignore_107 = ((chan_106 < 0) || (chan_106 >= amplitudes_channels));
			t_sample sample_amplitudes_509 = (chan_ignore_107 ? 0 : m_amplitudes_10.read(0, chan_106));
			t_sample index_amplitudes_488 = ((int)0);
			t_sample mul_497 = (mul_333 * sample_amplitudes_509);
			bool index_ignore_108 = (((int)1) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_109 = ((int)0);
			bool chan_ignore_110 = ((chan_109 < 0) || (chan_109 >= amplitudes_channels));
			t_sample sample_amplitudes_485 = ((chan_ignore_110 || index_ignore_108) ? 0 : m_amplitudes_10.read(((int)1), chan_109));
			t_sample index_amplitudes_516 = ((int)1);
			t_sample mul_520 = (mul_333 * sample_amplitudes_485);
			bool index_ignore_111 = (((int)3) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_112 = ((int)0);
			bool chan_ignore_113 = ((chan_112 < 0) || (chan_112 >= amplitudes_channels));
			t_sample sample_amplitudes_484 = ((chan_ignore_113 || index_ignore_111) ? 0 : m_amplitudes_10.read(((int)3), chan_112));
			t_sample index_amplitudes_486 = ((int)3);
			t_sample mul_501 = (mul_333 * sample_amplitudes_484);
			bool index_ignore_114 = (((int)2) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_115 = ((int)0);
			bool chan_ignore_116 = ((chan_115 < 0) || (chan_115 >= amplitudes_channels));
			t_sample sample_amplitudes_489 = ((chan_ignore_116 || index_ignore_114) ? 0 : m_amplitudes_10.read(((int)2), chan_115));
			t_sample index_amplitudes_502 = ((int)2);
			t_sample mul_495 = (mul_333 * sample_amplitudes_489);
			t_sample mul_334 = (tap_336 * m_fb_13);
			t_sample mul_249 = (m_spread_17 * ((int)3));
			t_sample mul_255 = (m_rate_15 * mul_249);
			t_sample add_254 = (m_rate_15 + mul_255);
			__m_cycle_117.freq(add_254);
			t_sample cycle_260 = __m_cycle_117(__sinedata);
			t_sample cycleindex_261 = __m_cycle_117.phase();
			t_sample mul_257 = (m_time_16 * mul_249);
			t_sample add_256 = (m_time_16 + mul_257);
			t_sample mstosamps_269 = (add_256 * (samplerate * 0.001));
			t_sample mix_648 = (m_history_2 + (((t_sample)0.002617990887418) * (mstosamps_269 - m_history_2)));
			t_sample mix_262 = mix_648;
			t_sample gen_268 = mix_262;
			t_sample history_264_next_267 = fixdenorm(mix_262);
			t_sample mul_259 = (cycle_260 * gen_268);
			t_sample mul_253 = (gen_268 * ((t_sample)0.1));
			t_sample add_252 = (gen_268 + mul_253);
			t_sample add_258 = (mul_259 + add_252);
			t_sample tap_332 = m_delay_1.read_cubic(add_258);
			t_sample mul_329 = (tap_332 * m_delGain_14);
			t_sample y_loc_536 = ((mul_251 <= ((int)-1)) ? ((int)-1) : ((mul_251 >= ((int)1)) ? ((int)1) : mul_251));
			t_sample x_loc_535 = ((mul_251 <= ((int)-1)) ? ((int)-1) : ((mul_251 >= ((int)1)) ? ((int)1) : mul_251));
			t_sample rotate_558 = ((m_rot_wet_12 <= ((int)0)) ? ((int)0) : ((m_rot_wet_12 >= ((int)1)) ? ((int)1) : m_rot_wet_12));
			t_sample atten_533 = ((m_wet_21 <= ((int)0)) ? ((int)0) : ((m_wet_21 >= ((int)1)) ? ((int)1) : m_wet_21));
			t_sample cartopol_649 = sqrt(((x_loc_535 * x_loc_535) + (y_loc_536 * y_loc_536)));
			t_sample cartopol_650 = atan2(y_loc_536, x_loc_535);
			t_sample dist_526 = ((cartopol_649 <= ((int)0)) ? ((int)0) : ((cartopol_649 >= ((int)1)) ? ((int)1) : cartopol_649));
			t_sample angle_rot_559 = (cartopol_650 + (((t_sample)6.2831853071796) * rotate_558));
			t_sample poltocar_651 = (dist_526 * cos(angle_rot_559));
			t_sample poltocar_652 = (dist_526 * sin(angle_rot_559));
			t_sample x_rad_sm_554 = smooth_524_d(poltocar_651);
			t_sample y_rad_sm_555 = smooth_524_d_119(poltocar_652);
			// for loop initializer;
			int i_531 = ((int)0);
			// for loop condition;
			while ((i_531 < ((int)4))) {
				// abort processing if an infinite loop is suspected;
				if (((__loopcount--) <= 0)) {
					__exception = GENLIB_ERR_LOOP_OVERFLOW;
					break ;
					
				};
				int si_543 = (((int)1) + (i_531 * ((int)2)));
				int speaker_loc_dim = m_speaker_loc_23.dim;
				int speaker_loc_channels = m_speaker_loc_23.channels;
				bool index_ignore_121 = ((si_543 >= speaker_loc_dim) || (si_543 < 0));
				// samples speaker_loc channel 1;
				int chan_122 = ((int)0);
				bool chan_ignore_123 = ((chan_122 < 0) || (chan_122 >= speaker_loc_channels));
				t_sample x_spk_546 = ((chan_ignore_123 || index_ignore_121) ? 0 : m_speaker_loc_23.read(si_543, chan_122));
				bool index_ignore_124 = (((si_543 + ((int)1)) >= speaker_loc_dim) || ((si_543 + ((int)1)) < 0));
				// samples speaker_loc channel 1;
				int chan_125 = ((int)0);
				bool chan_ignore_126 = ((chan_125 < 0) || (chan_125 >= speaker_loc_channels));
				t_sample y_spk_545 = ((chan_ignore_126 || index_ignore_124) ? 0 : m_speaker_loc_23.read((si_543 + ((int)1)), chan_125));
				t_sample dx_560 = (x_rad_sm_554 - x_spk_546);
				t_sample dy_542 = (y_rad_sm_555 - y_spk_545);
				t_sample v_127 = sqrt(((dx_560 * dx_560) + (dy_542 * dy_542)));
				t_sample distance_563 = ((v_127 <= ((int)0)) ? ((int)0) : ((v_127 >= ((t_sample)1.414)) ? ((t_sample)1.414) : v_127));
				t_sample sub_656 = (distance_563 - ((int)0));
				t_sample scale_653 = ((safepow((sub_656 * ((t_sample)0.70721357850071)), ((int)1)) * ((t_sample)-1.5707963267949)) + ((t_sample)1.5707963267949));
				t_sample amp_528 = (atten_533 * sin(scale_653));
				bool index_ignore_128 = ((i_531 >= amplitudes_dim) || (i_531 < 0));
				if ((!index_ignore_128)) {
					m_amplitudes_10.write(amp_528, i_531, 0);
					
				};
				// for loop increment;
				i_531 = (i_531 + ((int)1));
				
			};
			// samples amplitudes channel 1;
			int chan_129 = ((int)0);
			bool chan_ignore_130 = ((chan_129 < 0) || (chan_129 >= amplitudes_channels));
			t_sample sample_amplitudes_532 = (chan_ignore_130 ? 0 : m_amplitudes_10.read(0, chan_129));
			t_sample index_amplitudes_557 = ((int)0);
			t_sample mul_544 = (mul_329 * sample_amplitudes_532);
			bool index_ignore_131 = (((int)1) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_132 = ((int)0);
			bool chan_ignore_133 = ((chan_132 < 0) || (chan_132 >= amplitudes_channels));
			t_sample sample_amplitudes_538 = ((chan_ignore_133 || index_ignore_131) ? 0 : m_amplitudes_10.read(((int)1), chan_132));
			t_sample index_amplitudes_556 = ((int)1);
			t_sample mul_551 = (mul_329 * sample_amplitudes_538);
			bool index_ignore_134 = (((int)3) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_135 = ((int)0);
			bool chan_ignore_136 = ((chan_135 < 0) || (chan_135 >= amplitudes_channels));
			t_sample sample_amplitudes_537 = ((chan_ignore_136 || index_ignore_134) ? 0 : m_amplitudes_10.read(((int)3), chan_135));
			t_sample index_amplitudes_539 = ((int)3);
			t_sample mul_561 = (mul_329 * sample_amplitudes_537);
			bool index_ignore_137 = (((int)2) >= amplitudes_dim);
			// samples amplitudes channel 1;
			int chan_138 = ((int)0);
			bool chan_ignore_139 = ((chan_138 < 0) || (chan_138 >= amplitudes_channels));
			t_sample sample_amplitudes_540 = ((chan_ignore_139 || index_ignore_137) ? 0 : m_amplitudes_10.read(((int)2), chan_138));
			t_sample index_amplitudes_547 = ((int)2);
			t_sample mul_529 = (mul_329 * sample_amplitudes_540);
			t_sample out1 = ((((mul_353 + mul_544) + mul_497) + mul_455) + mul_424);
			t_sample out2 = ((((mul_349 + mul_551) + mul_520) + mul_478) + mul_404);
			t_sample out3 = ((((mul_345 + mul_529) + mul_495) + mul_453) + mul_422);
			t_sample out4 = ((((mul_341 + mul_561) + mul_501) + mul_458) + mul_430);
			t_sample mul_330 = (tap_332 * m_fb_13);
			m_history_8 = history_322_next_325;
			m_delay_7.write((mul_364 + in1));
			m_history_6 = history_303_next_306;
			m_delay_5.write((mul_394 + in1));
			m_history_4 = history_283_next_286;
			m_delay_3.write((mul_334 + in1));
			m_history_2 = history_264_next_267;
			m_delay_1.write((mul_330 + in1));
			m_delay_1.step();
			m_delay_3.step();
			m_delay_5.step();
			m_delay_7.step();
			// assign results to output buffer;
			(*(__out1++)) = out1;
			(*(__out2++)) = out2;
			(*(__out3++)) = out3;
			(*(__out4++)) = out4;
			
		};
		return __exception;
		
	};
	inline void set_tospeakers(void * _value) {
		m_tospeakers_9.setbuffer(_value);
	};
	inline void set_amplitudes(void * _value) {
		m_amplitudes_10.setbuffer(_value);
	};
	inline void set_wet_loc(t_param _value) {
		m_wet_loc_11 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_rot_wet(t_param _value) {
		m_rot_wet_12 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_fb(t_param _value) {
		m_fb_13 = (_value < 0 ? 0 : (_value > 0.99 ? 0.99 : _value));
	};
	inline void set_delGain(t_param _value) {
		m_delGain_14 = (_value < 0.1 ? 0.1 : (_value > 12 ? 12 : _value));
	};
	inline void set_rate(t_param _value) {
		m_rate_15 = (_value < 0.01 ? 0.01 : (_value > 30 ? 30 : _value));
	};
	inline void set_time(t_param _value) {
		m_time_16 = (_value < 1 ? 1 : (_value > 40 ? 40 : _value));
	};
	inline void set_spread(t_param _value) {
		m_spread_17 = (_value < 0 ? 0 : (_value > 4 ? 4 : _value));
	};
	inline void set_rot_dry(t_param _value) {
		m_rot_dry_18 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_dry(t_param _value) {
		m_dry_19 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_dry_loc(t_param _value) {
		m_dry_loc_20 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_wet(t_param _value) {
		m_wet_21 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_speaker_loc(void * _value) {
		m_speaker_loc_23.setbuffer(_value);
	};
	inline t_sample smooth_d(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_26 = (m_prev_26 + (incr * (v - m_prev_26)));
		return m_prev_26;
		
	};
	inline t_sample smooth_d_27(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_28 = (m_prev_28 + (incr * (v - m_prev_28)));
		return m_prev_28;
		
	};
	inline t_sample smooth_398_d(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_49 = (m_prev_49 + (incr * (v - m_prev_49)));
		return m_prev_49;
		
	};
	inline t_sample smooth_398_d_50(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_51 = (m_prev_51 + (incr * (v - m_prev_51)));
		return m_prev_51;
		
	};
	inline t_sample smooth_440_d(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_72 = (m_prev_72 + (incr * (v - m_prev_72)));
		return m_prev_72;
		
	};
	inline t_sample smooth_440_d_73(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_74 = (m_prev_74 + (incr * (v - m_prev_74)));
		return m_prev_74;
		
	};
	inline t_sample smooth_482_d(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_95 = (m_prev_95 + (incr * (v - m_prev_95)));
		return m_prev_95;
		
	};
	inline t_sample smooth_482_d_96(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_97 = (m_prev_97 + (incr * (v - m_prev_97)));
		return m_prev_97;
		
	};
	inline t_sample smooth_524_d(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_118 = (m_prev_118 + (incr * (v - m_prev_118)));
		return m_prev_118;
		
	};
	inline t_sample smooth_524_d_119(t_sample v) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_120 = (m_prev_120 + (incr * (v - m_prev_120)));
		return m_prev_120;
		
	};
	
} State;


///
///	Configuration for the genlib API
///

/// Number of signal inputs and outputs

int gen_kernel_numins = 1;
int gen_kernel_numouts = 4;

int num_inputs() { return gen_kernel_numins; }
int num_outputs() { return gen_kernel_numouts; }
int num_params() { return 14; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "audio_in" };
const char *gen_kernel_outnames[] = { "out1", "out2", "out3", "out4" };

/// Invoke the signal process of a State object

int perform(CommonState *cself, t_sample **ins, long numins, t_sample **outs, long numouts, long n) {
	State* self = (State *)cself;
	return self->perform(ins, outs, n);
}

/// Reset all parameters and stateful operators of a State object

void reset(CommonState *cself) {
	State* self = (State *)cself;
	self->reset(cself->sr, cself->vs);
}

/// Set a parameter of a State object

void setparameter(CommonState *cself, long index, t_param value, void *ref) {
	State *self = (State *)cself;
	switch (index) {
		case 0: self->set_amplitudes(ref); break;
		case 1: self->set_delGain(value); break;
		case 2: self->set_dry(value); break;
		case 3: self->set_dry_loc(value); break;
		case 4: self->set_fb(value); break;
		case 5: self->set_rate(value); break;
		case 6: self->set_rot_dry(value); break;
		case 7: self->set_rot_wet(value); break;
		case 8: self->set_speaker_loc(ref); break;
		case 9: self->set_spread(value); break;
		case 10: self->set_time(value); break;
		case 11: self->set_tospeakers(ref); break;
		case 12: self->set_wet(value); break;
		case 13: self->set_wet_loc(value); break;
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		
		case 1: *value = self->m_delGain_14; break;
		case 2: *value = self->m_dry_19; break;
		case 3: *value = self->m_dry_loc_20; break;
		case 4: *value = self->m_fb_13; break;
		case 5: *value = self->m_rate_15; break;
		case 6: *value = self->m_rot_dry_18; break;
		case 7: *value = self->m_rot_wet_12; break;
		
		case 9: *value = self->m_spread_17; break;
		case 10: *value = self->m_time_16; break;
		
		case 12: *value = self->m_wet_21; break;
		case 13: *value = self->m_wet_loc_11; break;
		
		default: break;
	}
}

/// Get the name of a parameter of a State object

const char *getparametername(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].name;
	}
	return 0;
}

/// Get the minimum value of a parameter of a State object

t_param getparametermin(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmin;
	}
	return 0;
}

/// Get the maximum value of a parameter of a State object

t_param getparametermax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].outputmax;
	}
	return 0;
}

/// Get parameter of a State object has a minimum and maximum value

char getparameterhasminmax(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].hasminmax;
	}
	return 0;
}

/// Get the units of a parameter of a State object

const char *getparameterunits(CommonState *cself, long index) {
	if (index >= 0 && index < cself->numparams) {
		return cself->params[index].units;
	}
	return 0;
}

/// Get the size of the state of all parameters of a State object

size_t getstatesize(CommonState *cself) {
	return genlib_getstatesize(cself, &getparameter);
}

/// Get the state of all parameters of a State object

short getstate(CommonState *cself, char *state) {
	return genlib_getstate(cself, state, &getparameter);
}

/// set the state of all parameters of a State object

short setstate(CommonState *cself, const char *state) {
	return genlib_setstate(cself, state, &setparameter);
}

/// Allocate and configure a new State object and it's internal CommonState:

void *create(t_param sr, long vs) {
	State *self = new State;
	self->reset(sr, vs);
	ParamInfo *pi;
	self->__commonstate.inputnames = gen_kernel_innames;
	self->__commonstate.outputnames = gen_kernel_outnames;
	self->__commonstate.numins = gen_kernel_numins;
	self->__commonstate.numouts = gen_kernel_numouts;
	self->__commonstate.sr = sr;
	self->__commonstate.vs = vs;
	self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(14 * sizeof(ParamInfo));
	self->__commonstate.numparams = 14;
	// initialize parameter 0 ("m_amplitudes_10")
	pi = self->__commonstate.params + 0;
	pi->name = "amplitudes";
	pi->paramtype = GENLIB_PARAMTYPE_SYM;
	pi->defaultvalue = 0.;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = false;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 1 ("m_delGain_14")
	pi = self->__commonstate.params + 1;
	pi->name = "delGain";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_delGain_14;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0.1;
	pi->outputmax = 12;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_dry_19")
	pi = self->__commonstate.params + 2;
	pi->name = "dry";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_dry_19;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_dry_loc_20")
	pi = self->__commonstate.params + 3;
	pi->name = "dry_loc";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_dry_loc_20;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 4 ("m_fb_13")
	pi = self->__commonstate.params + 4;
	pi->name = "fb";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_fb_13;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 0.99;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 5 ("m_rate_15")
	pi = self->__commonstate.params + 5;
	pi->name = "rate";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_rate_15;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0.01;
	pi->outputmax = 30;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 6 ("m_rot_dry_18")
	pi = self->__commonstate.params + 6;
	pi->name = "rot_dry";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_rot_dry_18;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 7 ("m_rot_wet_12")
	pi = self->__commonstate.params + 7;
	pi->name = "rot_wet";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_rot_wet_12;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 8 ("m_speaker_loc_23")
	pi = self->__commonstate.params + 8;
	pi->name = "speaker_loc";
	pi->paramtype = GENLIB_PARAMTYPE_SYM;
	pi->defaultvalue = 0.;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = false;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 9 ("m_spread_17")
	pi = self->__commonstate.params + 9;
	pi->name = "spread";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_spread_17;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 4;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 10 ("m_time_16")
	pi = self->__commonstate.params + 10;
	pi->name = "time";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_time_16;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 1;
	pi->outputmax = 40;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 11 ("m_tospeakers_9")
	pi = self->__commonstate.params + 11;
	pi->name = "tospeakers";
	pi->paramtype = GENLIB_PARAMTYPE_SYM;
	pi->defaultvalue = 0.;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = false;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 12 ("m_wet_21")
	pi = self->__commonstate.params + 12;
	pi->name = "wet";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_wet_21;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 13 ("m_wet_loc_11")
	pi = self->__commonstate.params + 13;
	pi->name = "wet_loc";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_wet_loc_11;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	
	return self;
}

/// Release all resources and memory used by a State object:

void destroy(CommonState *cself) {
	State *self = (State *)cself;
	genlib_sysmem_freeptr(cself->params);
		
	delete self;
}


} // chorus_flange::
