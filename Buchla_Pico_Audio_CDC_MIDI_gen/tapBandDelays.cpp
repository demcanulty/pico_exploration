#include "tapBandDelays.h"

namespace tapBandDelays {

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
	Delay m_delay_11;
	Delay m_delay_14;
	Delay m_delay_8;
	Delay m_delay_5;
	int __exception;
	int vectorsize;
	t_sample m_d_13;
	t_sample m_d_16;
	t_sample m_history_17;
	t_sample m_history_19;
	t_sample m_d_12;
	t_sample m_history_18;
	t_sample m_d_15;
	t_sample m_d_9;
	t_sample m_history_20;
	t_sample m_DTIME_ms_2;
	t_sample m_Q_1;
	t_sample samplerate;
	t_sample m_d_10;
	t_sample m_FREQ_3;
	t_sample m_d_6;
	t_sample m_FB_4;
	t_sample m_d_7;
	t_sample m_prev_21;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_Q_1 = ((t_sample)0.5);
		m_DTIME_ms_2 = ((int)1000);
		m_FREQ_3 = ((int)1000);
		m_FB_4 = ((t_sample)0.5);
		m_delay_5.reset("m_delay_5", (samplerate * 2));
		m_d_6 = ((int)0);
		m_d_7 = ((int)0);
		m_delay_8.reset("m_delay_8", (samplerate * 2));
		m_d_9 = ((int)0);
		m_d_10 = ((int)0);
		m_delay_11.reset("m_delay_11", (samplerate * 2));
		m_d_12 = ((int)0);
		m_d_13 = ((int)0);
		m_delay_14.reset("m_delay_14", (samplerate * 2));
		m_d_15 = ((int)0);
		m_d_16 = ((int)0);
		m_history_17 = ((int)0);
		m_history_18 = ((int)0);
		m_history_19 = ((int)0);
		m_history_20 = ((int)0);
		m_prev_21 = ((int)0);
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
		t_sample mul_494 = (m_FREQ_3 * ((t_sample)0.2));
		t_sample mul_475 = (m_FREQ_3 * ((t_sample)0.5));
		t_sample mul_493 = (m_FREQ_3 * ((int)2));
		t_sample freq = m_FREQ_3;
		t_sample res = m_Q_1;
		t_sample q1 = safediv(((int)1), res);
		t_sample f1 = safediv((((t_sample)6.2831853071796) * freq), samplerate);
		t_sample freq_531 = mul_494;
		t_sample res_534 = m_Q_1;
		t_sample q_538 = safediv(((int)1), res_534);
		t_sample f_529 = safediv((((t_sample)6.2831853071796) * freq_531), samplerate);
		t_sample freq_543 = mul_475;
		t_sample res_546 = m_Q_1;
		t_sample q_550 = safediv(((int)1), res_546);
		t_sample f_541 = safediv((((t_sample)6.2831853071796) * freq_543), samplerate);
		t_sample freq_556 = mul_493;
		t_sample res_562 = m_Q_1;
		t_sample q_555 = safediv(((int)1), res_562);
		t_sample f_553 = safediv((((t_sample)6.2831853071796) * freq_556), samplerate);
		// the main sample loop;
		while ((__n--)) {
			const t_sample in1 = (*(__in1++));
			t_sample mul_512 = (m_history_17 * m_FB_4);
			t_sample mul_496 = (m_history_18 * m_FB_4);
			t_sample mul_477 = (m_history_19 * m_FB_4);
			t_sample mul_459 = (m_history_20 * m_FB_4);
			t_sample expr_456 = smooth_d_i(m_DTIME_ms_2, ((int)0));
			t_sample gen_457 = expr_456;
			t_sample mstosamps_528 = (gen_457 * (samplerate * 0.001));
			t_sample tap_527 = m_delay_5.read_cubic(mstosamps_528);
			t_sample L = (m_d_7 + (f1 * m_d_6));
			t_sample H = ((tap_527 - L) - (q1 * m_d_6));
			t_sample B = ((f1 * H) + m_d_6);
			m_d_6 = B;
			m_d_7 = L;
			t_sample expr_520 = B;
			t_sample gen_524 = expr_520;
			t_sample out1 = gen_524;
			t_sample div_495 = (mstosamps_528 * ((t_sample)0.33333333333333));
			t_sample tap_511 = m_delay_8.read_cubic(div_495);
			t_sample L_537 = (m_d_10 + (f_529 * m_d_9));
			t_sample H_533 = ((tap_511 - L_537) - (q_538 * m_d_9));
			t_sample B_532 = ((f_529 * H_533) + m_d_9);
			m_d_9 = B_532;
			m_d_10 = L_537;
			t_sample expr_504 = B_532;
			t_sample gen_508 = expr_504;
			t_sample out2 = gen_508;
			t_sample div_476 = (mstosamps_528 * ((t_sample)0.5));
			t_sample tap_492 = m_delay_11.read_cubic(div_476);
			t_sample L_549 = (m_d_13 + (f_541 * m_d_12));
			t_sample H_545 = ((tap_492 - L_549) - (q_550 * m_d_12));
			t_sample B_544 = ((f_541 * H_545) + m_d_12);
			m_d_12 = B_544;
			m_d_13 = L_549;
			t_sample expr_485 = B_544;
			t_sample gen_489 = expr_485;
			t_sample out3 = gen_489;
			t_sample mul_458 = (mstosamps_528 * ((t_sample)0.75));
			t_sample tap_474 = m_delay_14.read_cubic(mul_458);
			t_sample L_558 = (m_d_16 + (f_553 * m_d_15));
			t_sample H_557 = ((tap_474 - L_558) - (q_555 * m_d_15));
			t_sample B_561 = ((f_553 * H_557) + m_d_15);
			m_d_15 = B_561;
			m_d_16 = L_558;
			t_sample expr_467 = B_561;
			t_sample gen_471 = expr_467;
			t_sample out4 = gen_471;
			t_sample history_513_next_565 = fixdenorm(gen_524);
			t_sample history_497_next_566 = fixdenorm(gen_508);
			t_sample history_478_next_567 = fixdenorm(gen_489);
			t_sample history_460_next_568 = fixdenorm(gen_471);
			m_delay_5.write((mul_512 + in1));
			m_history_20 = history_460_next_568;
			m_history_19 = history_478_next_567;
			m_history_18 = history_497_next_566;
			m_history_17 = history_513_next_565;
			m_delay_14.write((mul_459 + in1));
			m_delay_11.write((mul_477 + in1));
			m_delay_8.write((mul_496 + in1));
			m_delay_5.step();
			m_delay_8.step();
			m_delay_11.step();
			m_delay_14.step();
			// assign results to output buffer;
			(*(__out1++)) = out1;
			(*(__out2++)) = out2;
			(*(__out3++)) = out3;
			(*(__out4++)) = out4;
			
		};
		return __exception;
		
	};
	inline void set_Q(t_param _value) {
		m_Q_1 = (_value < 0.1 ? 0.1 : (_value > 5 ? 5 : _value));
	};
	inline void set_DTIME_ms(t_param _value) {
		m_DTIME_ms_2 = (_value < 0 ? 0 : (_value > 2000 ? 2000 : _value));
	};
	inline void set_FREQ(t_param _value) {
		m_FREQ_3 = (_value < 20 ? 20 : (_value > 3500 ? 3500 : _value));
	};
	inline void set_FB(t_param _value) {
		m_FB_4 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline t_sample smooth_d_i(t_sample v, int a) {
		t_sample incr = safediv(((int)250), samplerate);
		m_prev_21 = (m_prev_21 + (incr * (v - m_prev_21)));
		return m_prev_21;
		
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
int num_params() { return 4; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "audio" };
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
		case 0: self->set_DTIME_ms(value); break;
		case 1: self->set_FB(value); break;
		case 2: self->set_FREQ(value); break;
		case 3: self->set_Q(value); break;
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		case 0: *value = self->m_DTIME_ms_2; break;
		case 1: *value = self->m_FB_4; break;
		case 2: *value = self->m_FREQ_3; break;
		case 3: *value = self->m_Q_1; break;
		
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
	self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(4 * sizeof(ParamInfo));
	self->__commonstate.numparams = 4;
	// initialize parameter 0 ("m_DTIME_ms_2")
	pi = self->__commonstate.params + 0;
	pi->name = "DTIME_ms";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_DTIME_ms_2;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 2000;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 1 ("m_FB_4")
	pi = self->__commonstate.params + 1;
	pi->name = "FB";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_FB_4;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_FREQ_3")
	pi = self->__commonstate.params + 2;
	pi->name = "FREQ";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_FREQ_3;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 20;
	pi->outputmax = 3500;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_Q_1")
	pi = self->__commonstate.params + 3;
	pi->name = "Q";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_Q_1;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0.1;
	pi->outputmax = 5;
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


} // tapBandDelays::
