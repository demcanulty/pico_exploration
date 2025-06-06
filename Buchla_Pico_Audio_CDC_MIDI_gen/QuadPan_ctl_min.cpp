#include "QuadPan_ctl_min.h"

namespace QuadPan_ctl_min {

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
	Data m_amplitudes_3;
	Data m_speaker_loc_2;
	int __exception;
	int __loopcount;
	int vectorsize;
	t_sample m_rotate_6;
	t_sample m_x_loc_5;
	t_sample m_y_loc_4;
	t_sample m_atten_7;
	t_sample samplerate;
	t_sample m_loadbang_1;
	// re-initialize all member variables;
	inline void reset(t_param __sr, int __vs) {
		__exception = 0;
		vectorsize = __vs;
		samplerate = __sr;
		m_loadbang_1 = ((int)1);
		m_speaker_loc_2.reset("speaker_loc", ((int)9), ((int)1));
		m_amplitudes_3.reset("amplitudes", ((int)4), ((int)1));
		m_y_loc_4 = ((int)0);
		m_x_loc_5 = ((int)0);
		m_rotate_6 = ((int)0);
		m_atten_7 = ((t_sample)0.1);
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
		t_sample cartopol_584 = sqrt(((m_x_loc_5 * m_x_loc_5) + (m_y_loc_4 * m_y_loc_4)));
		t_sample cartopol_585 = atan2(m_y_loc_4, m_x_loc_5);
		t_sample dist_450 = ((cartopol_584 <= ((int)0)) ? ((int)0) : ((cartopol_584 >= ((int)1)) ? ((int)1) : cartopol_584));
		t_sample angle_rot = (cartopol_585 + (((t_sample)6.2831853071796) * m_rotate_6));
		t_sample poltocar_586 = (dist_450 * cos(angle_rot));
		t_sample poltocar_587 = (dist_450 * sin(angle_rot));
		int amplitudes_dim = m_amplitudes_3.dim;
		//int amplitudes_channels = m_amplitudes_3.channels;
		bool index_ignore_14 = (((int)1) >= amplitudes_dim);
		bool index_ignore_15 = (((int)2) >= amplitudes_dim);
		bool index_ignore_16 = (((int)3) >= amplitudes_dim);
		// the main sample loop;
		while ((__n--)) {
			//const t_sample in1 = (*(__in1++));
			if (m_loadbang_1) {
				// for loop initializer;
				int i = ((int)1);
				// for loop condition;
				while ((i <= ((int)8))) {
					// abort processing if an infinite loop is suspected;
					if (((__loopcount--) <= 0)) {
						__exception = GENLIB_ERR_LOOP_OVERFLOW;
						break ;
						
					};
					int speaker_loc_dim = m_speaker_loc_2.dim;
					//int speaker_loc_channels = m_speaker_loc_2.channels;
					bool index_ignore_8 = ((i >= speaker_loc_dim) || (i < 0));
					if ((!index_ignore_8)) {
						int choice_9 = i;
						m_speaker_loc_2.write(((choice_9 >= 8) ? ((t_sample)-0.707) : ((choice_9 >= 7) ? ((t_sample)-0.707) : ((choice_9 >= 6) ? ((t_sample)-0.707) : ((choice_9 >= 5) ? ((t_sample)0.707) : ((choice_9 >= 4) ? ((t_sample)0.707) : ((choice_9 >= 3) ? ((t_sample)0.707) : ((choice_9 >= 2) ? ((t_sample)0.707) : ((choice_9 >= 1) ? ((t_sample)-0.707) : 0)))))))), i, 0);
						
					};
					// for loop increment;
					i = (i + ((int)1));
					
				};
				m_loadbang_1 = ((int)0);
				
			};
			// abort processing if an infinite loop is suspected;
			if (((__loopcount--) <= 0)) {
				__exception = GENLIB_ERR_LOOP_OVERFLOW;
				break ;
				
			};
			// for loop initializer;
			int i = ((int)0);
			// for loop condition;
			while ((i < ((int)4))) {
				// abort processing if an infinite loop is suspected;
				if (((__loopcount--) <= 0)) {
					__exception = GENLIB_ERR_LOOP_OVERFLOW;
					break ;
					
				};
				int si = (((int)1) + (i * ((int)2)));
				int speaker_loc_dim = m_speaker_loc_2.dim;
				//int speaker_loc_channels = m_speaker_loc_2.channels;
				bool index_ignore_10 = ((si >= speaker_loc_dim) || (si < 0));
				// samples speaker_loc channel 1;
				t_sample x_spk = (index_ignore_10 ? 0 : m_speaker_loc_2.read(si, 0));
				bool index_ignore_11 = (((si + ((int)1)) >= speaker_loc_dim) || ((si + ((int)1)) < 0));
				// samples speaker_loc channel 1;
				t_sample y_spk = (index_ignore_11 ? 0 : m_speaker_loc_2.read((si + ((int)1)), 0));
				t_sample dx = (poltocar_586 - x_spk);
				t_sample dy = (poltocar_587 - y_spk);
				t_sample v_12 = sqrt(((dx * dx) + (dy * dy)));
				t_sample distance = ((v_12 <= ((int)0)) ? ((int)0) : ((v_12 >= ((t_sample)1.414)) ? ((t_sample)1.414) : v_12));
				t_sample sub_591 = (distance - ((int)0));
				t_sample scale_588 = ((safepow((sub_591 * ((t_sample)0.70721357850071)), ((int)1)) * ((t_sample)-1.5707963267949)) + ((t_sample)1.5707963267949));
				t_sample amp = (m_atten_7 * sin(scale_588));
				int amplitudes_dim = m_amplitudes_3.dim;
				//int amplitudes_channels = m_amplitudes_3.channels;
				bool index_ignore_13 = ((i >= amplitudes_dim) || (i < 0));
				if ((!index_ignore_13)) {
					m_amplitudes_3.write(amp, i, 0);
					
				};
				// for loop increment;
				i = (i + ((int)1));
				
			};
			// samples amplitudes channel 1;
			t_sample expr_451 = m_amplitudes_3.read(0, 0);
			// samples amplitudes channel 1;
			t_sample expr_452 = (index_ignore_14 ? 0 : m_amplitudes_3.read(((int)1), 0));
			// samples amplitudes channel 1;
			t_sample expr_453 = (index_ignore_15 ? 0 : m_amplitudes_3.read(((int)2), 0));
			// samples amplitudes channel 1;
			t_sample expr_454 = (index_ignore_16 ? 0 : m_amplitudes_3.read(((int)3), 0));
			t_sample out2 = expr_452;
			t_sample out4 = expr_454;
			t_sample out1 = expr_451;
			t_sample out3 = expr_453;
			// assign results to output buffer;
			(*(__out1++)) = out1;
			(*(__out2++)) = out2;
			(*(__out3++)) = out3;
			(*(__out4++)) = out4;
			
		};
		return __exception;
		
	};
	inline void set_speaker_loc(void * _value) {
		m_speaker_loc_2.setbuffer(_value);
	};
	inline void set_amplitudes(void * _value) {
		m_amplitudes_3.setbuffer(_value);
	};
	inline void set_y_loc(t_param _value) {
		m_y_loc_4 = (_value < -1 ? -1 : (_value > 1 ? 1 : _value));
	};
	inline void set_x_loc(t_param _value) {
		m_x_loc_5 = (_value < -1 ? -1 : (_value > 1 ? 1 : _value));
	};
	inline void set_rotate(t_param _value) {
		m_rotate_6 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
	};
	inline void set_atten(t_param _value) {
		m_atten_7 = (_value < 0 ? 0 : (_value > 1 ? 1 : _value));
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
int num_params() { return 6; }

/// Assistive lables for the signal inputs and outputs

const char *gen_kernel_innames[] = { "in1" };
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
		case 1: self->set_atten(value); break;
		case 2: self->set_rotate(value); break;
		case 3: self->set_speaker_loc(ref); break;
		case 4: self->set_x_loc(value); break;
		case 5: self->set_y_loc(value); break;
		
		default: break;
	}
}

/// Get the value of a parameter of a State object

void getparameter(CommonState *cself, long index, t_param *value) {
	State *self = (State *)cself;
	switch (index) {
		
		case 1: *value = self->m_atten_7; break;
		case 2: *value = self->m_rotate_6; break;
		
		case 4: *value = self->m_x_loc_5; break;
		case 5: *value = self->m_y_loc_4; break;
		
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
	self->__commonstate.params = (ParamInfo *)genlib_sysmem_newptr(6 * sizeof(ParamInfo));
	self->__commonstate.numparams = 6;
	// initialize parameter 0 ("m_amplitudes_3")
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
	// initialize parameter 1 ("m_atten_7")
	pi = self->__commonstate.params + 1;
	pi->name = "atten";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_atten_7;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 2 ("m_rotate_6")
	pi = self->__commonstate.params + 2;
	pi->name = "rotate";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_rotate_6;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = 0;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 3 ("m_speaker_loc_2")
	pi = self->__commonstate.params + 3;
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
	// initialize parameter 4 ("m_x_loc_5")
	pi = self->__commonstate.params + 4;
	pi->name = "x_loc";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_x_loc_5;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = -1;
	pi->outputmax = 1;
	pi->exp = 0;
	pi->units = "";		// no units defined
	// initialize parameter 5 ("m_y_loc_4")
	pi = self->__commonstate.params + 5;
	pi->name = "y_loc";
	pi->paramtype = GENLIB_PARAMTYPE_FLOAT;
	pi->defaultvalue = self->m_y_loc_4;
	pi->defaultref = 0;
	pi->hasinputminmax = false;
	pi->inputmin = 0;
	pi->inputmax = 1;
	pi->hasminmax = true;
	pi->outputmin = -1;
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


} // QuadPan_ctl_min::
