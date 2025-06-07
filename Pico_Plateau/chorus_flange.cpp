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
	Delay m_delay_7;
	Delay m_delay_1;
	Delay m_delay_3;
	Delay m_delay_5;
	SineCycle __m_cycle_16;
	SineCycle __m_cycle_17;
	SineCycle __m_cycle_15;
	SineCycle __m_cycle_14;
	SineData __sinedata;
	int __exception;
	int vectorsize;
	t_sample m_history_4;
	t_sample m_history_2;
	t_sample samplerate;
	t_sample m_history_6;
	t_sample m_spread_13;
	t_sample m_rate_11;
	t_sample m_fb_12;
	t_sample m_delGain_9;
	t_sample m_time_10;
	t_sample m_history_8;
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
		m_delGain_9 = ((int)1);
		m_time_10 = ((int)5);
		m_rate_11 = ((t_sample)0.2);
		m_fb_12 = ((t_sample)0.3);
		m_spread_13 = ((t_sample)0.47);
		__m_cycle_14.reset(samplerate, 0);
		__m_cycle_15.reset(samplerate, 0);
		__m_cycle_16.reset(samplerate, 0);
		__m_cycle_17.reset(samplerate, 0);
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
		t_sample mul_1041 = (m_rate_11 * m_spread_13);
		t_sample add_1040 = (m_rate_11 + mul_1041);
		t_sample mul_1043 = (m_time_10 * m_spread_13);
		t_sample add_1042 = (m_time_10 + mul_1043);
		t_sample mstosamps_1055 = (add_1042 * (samplerate * 0.001));
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			t_sample out3 = ((int)0);
			t_sample out4 = ((int)0);
			__m_cycle_14.freq(add_1040);
			t_sample cycle_1046 = __m_cycle_14(__sinedata);
			t_sample cycleindex_1047 = __m_cycle_14.phase();
			t_sample mix_2198 = (m_history_8 + (((t_sample)0.002617990887418) * (mstosamps_1055 - m_history_8)));
			t_sample mix_1048 = mix_2198;
			t_sample gen_1054 = mix_1048;
			t_sample history_1050_next_1053 = fixdenorm(mix_1048);
			t_sample mul_1045 = (cycle_1046 * gen_1054);
			t_sample mul_1039 = (gen_1054 * ((t_sample)0.1));
			t_sample add_1038 = (gen_1054 + mul_1039);
			t_sample add_1044 = (mul_1045 + add_1038);
			t_sample tap_1094 = m_delay_7.read_cubic(add_1044);
			t_sample mul_1091 = (tap_1094 * m_delGain_9);
			t_sample mul_1092 = (tap_1094 * m_fb_12);
			t_sample mul_1022 = (m_rate_11 * ((int)0));
			t_sample add_1021 = (m_rate_11 + mul_1022);
			__m_cycle_15.freq(add_1021);
			t_sample cycle_1027 = __m_cycle_15(__sinedata);
			t_sample cycleindex_1028 = __m_cycle_15.phase();
			t_sample mul_1024 = (m_time_10 * ((int)0));
			t_sample add_1023 = (m_time_10 + mul_1024);
			t_sample mstosamps_1036 = (add_1023 * (samplerate * 0.001));
			t_sample mix_2199 = (m_history_6 + (((t_sample)0.002617990887418) * (mstosamps_1036 - m_history_6)));
			t_sample mix_1029 = mix_2199;
			t_sample gen_1035 = mix_1029;
			t_sample history_1031_next_1034 = fixdenorm(mix_1029);
			t_sample mul_1026 = (cycle_1027 * gen_1035);
			t_sample mul_1020 = (gen_1035 * ((t_sample)0.1));
			t_sample add_1019 = (gen_1035 + mul_1020);
			t_sample add_1025 = (mul_1026 + add_1019);
			t_sample tap_1124 = m_delay_5.read_cubic(add_1025);
			t_sample mul_1121 = (tap_1124 * m_delGain_9);
			t_sample mul_1122 = (tap_1124 * m_fb_12);
			t_sample mul_978 = (m_spread_13 * ((int)2));
			t_sample mul_1002 = (m_rate_11 * mul_978);
			t_sample add_1001 = (m_rate_11 + mul_1002);
			__m_cycle_16.freq(add_1001);
			t_sample cycle_1007 = __m_cycle_16(__sinedata);
			t_sample cycleindex_1008 = __m_cycle_16.phase();
			t_sample mul_1004 = (m_time_10 * mul_978);
			t_sample add_1003 = (m_time_10 + mul_1004);
			t_sample mstosamps_1016 = (add_1003 * (samplerate * 0.001));
			t_sample mix_2200 = (m_history_4 + (((t_sample)0.002617990887418) * (mstosamps_1016 - m_history_4)));
			t_sample mix_1009 = mix_2200;
			t_sample gen_1015 = mix_1009;
			t_sample history_1011_next_1014 = fixdenorm(mix_1009);
			t_sample mul_1006 = (cycle_1007 * gen_1015);
			t_sample mul_1000 = (gen_1015 * ((t_sample)0.1));
			t_sample add_999 = (gen_1015 + mul_1000);
			t_sample add_1005 = (mul_1006 + add_999);
			t_sample tap_1064 = m_delay_3.read_cubic(add_1005);
			t_sample mul_1805 = (tap_1064 * m_delGain_9);
			t_sample out1 = (mul_1805 + mul_1121);
			t_sample mul_1062 = (tap_1064 * m_fb_12);
			t_sample mul_977 = (m_spread_13 * ((int)3));
			t_sample mul_983 = (m_rate_11 * mul_977);
			t_sample add_982 = (m_rate_11 + mul_983);
			__m_cycle_17.freq(add_982);
			t_sample cycle_988 = __m_cycle_17(__sinedata);
			t_sample cycleindex_989 = __m_cycle_17.phase();
			t_sample mul_985 = (m_time_10 * mul_977);
			t_sample add_984 = (m_time_10 + mul_985);
			t_sample mstosamps_997 = (add_984 * (samplerate * 0.001));
			t_sample mix_2201 = (m_history_2 + (((t_sample)0.002617990887418) * (mstosamps_997 - m_history_2)));
			t_sample mix_990 = mix_2201;
			t_sample gen_996 = mix_990;
			t_sample history_992_next_995 = fixdenorm(mix_990);
			t_sample mul_987 = (cycle_988 * gen_996);
			t_sample mul_981 = (gen_996 * ((t_sample)0.1));
			t_sample add_980 = (gen_996 + mul_981);
			t_sample add_986 = (mul_987 + add_980);
			t_sample tap_1060 = m_delay_1.read_cubic(add_986);
			t_sample mul_1804 = (tap_1060 * m_delGain_9);
			t_sample out2 = (mul_1804 + mul_1091);
			t_sample mul_1058 = (tap_1060 * m_fb_12);
			m_history_8 = history_1050_next_1053;
			m_delay_7.write((mul_1092 + in1));
			m_history_6 = history_1031_next_1034;
			m_delay_5.write((mul_1122 + in1));
			m_history_4 = history_1011_next_1014;
			m_delay_3.write((mul_1062 + in1));
			m_history_2 = history_992_next_995;
			m_delay_1.write((mul_1058 + in1));
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
	inline void set_delGain(t_param _value) {
		m_delGain_9 = (_value < 0.1 ? 0.1 : (_value > 12 ? 12 : _value));
	};
	inline void set_time(t_param _value) {
		m_time_10 = (_value < 1 ? 1 : (_value > 40 ? 40 : _value));
	};
	inline void set_rate(t_param _value) {
		m_rate_11 = (_value < 0.01 ? 0.01 : (_value > 30 ? 30 : _value));
	};
	inline void set_fb(t_param _value) {
		m_fb_12 = (_value < 0 ? 0 : (_value > 0.99 ? 0.99 : _value));
	};
	inline void set_spread(t_param _value) {
		m_spread_13 = (_value < 0 ? 0 : (_value > 4 ? 4 : _value));
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
int num_params() { return 5; }

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
		case 0: self->set_delGain(value); break;
		case 1: self->set_fb(value); break;
		case 2: self->set_rate(value); break;
		case 3: self->set_spread(value); break;
		case 4: self->set_time(value); break;
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		case 0: *value = self->m_delGain_9; break;
		case 1: *value = self->m_fb_12; break;
		case 2: *value = self->m_rate_11; break;
		case 3: *value = self->m_spread_13; break;
		case 4: *value = self->m_time_10; break;
		
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
	self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(5 * sizeof(ParamInfo));
	self->__commonstate.numparams = 5;
	// initialize parameter 0 ("m_delGain_9")
	pi = self->__commonstate.params + 0;
	pi->name = "delGain";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_delGain_9;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0.1;
	pi->outputmax = 12;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 1 ("m_fb_12")
	pi = self->__commonstate.params + 1;
	pi->name = "fb";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_fb_12;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 0.99;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_rate_11")
	pi = self->__commonstate.params + 2;
	pi->name = "rate";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_rate_11;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0.01;
	pi->outputmax = 30;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_spread_13")
	pi = self->__commonstate.params + 3;
	pi->name = "spread";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_spread_13;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 4;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 4 ("m_time_10")
	pi = self->__commonstate.params + 4;
	pi->name = "time";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_time_10;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 1;
	pi->outputmax = 40;
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
