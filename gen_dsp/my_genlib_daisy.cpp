
#include "daisy_seed.h"
#include "../main.h"

//***************************************************
//***************************************************
//***************************************************


#include "genlib.h"
#include "genlib_ops.h"
#include <stdlib.h> // not cstdlib (causes problems with ARM embedded compiler)
#include <cstdio>
#include <cstring>

#ifndef MSP_ON_CLANG
#	include <cmath>
#endif

#include <malloc.h>
#define malloc_size malloc_usable_size

// DATA_MAXIMUM_ELEMENTS * 8 bytes = 256 mb limit
#define DATA_MAXIMUM_ELEMENTS	(33554432)





#define DAISY_SEED_SRAM_SIZE    (512 * 1024)
#define DAISY_SEED_RAM_D2_DMA   (32 * 1024)
#define DAISY_SEED_SDRAM_SIZE    (64 * 1024 * 1024) // 1024

uint32_t sram_used  = 0, sram_usable  = 0;
uint32_t sdram_used = 0, sdram_usable = 0;
char * sram_pool = nullptr;
char DSY_SDRAM_BSS sdram_pool[DAISY_SEED_SDRAM_SIZE];

//****************************************
//****************************************
//****************************************

//***  THESE PULLED FROM genlib_daisy.h  

void * daisy_allocate(uint32_t size) 
{
    if (size < sram_usable) 
    {
        void * p = sram_pool + sram_used;
        sram_used += size;
        sram_usable -= size;
        return p;
    } 
    else if (size < sdram_usable) 
    {
        void * p = sdram_pool + sdram_used;
        sdram_used += size;
        sdram_usable -= size;
        return p;
    }
    return nullptr;
}	


void daisy_memset(void *p, int c, long size) 
{
    char *p2 = (char *)p;
    int i;

    for (i = 0; i < size; i++, p2++)
    {
        *p2 = char(c);
    } 
}


void daisy_memory_init() 
{
    if (!sram_pool) 
    {
        sram_pool = (char *)malloc(DAISY_SEED_RAM_D2_DMA);  //be careful about where malloc is pulling pointers from
    }
    // There's no guarantee the allocation will actually be
    // of size "OOPSY_SRAM_SIZE," so this just clamps the
    // usable space to what it really is.
    sram_usable = (0x30008000 - 1024) - ((size_t) sram_pool);
    sram_usable = 0;                                            //setting sram_usable to 0 causes the mem allocator to only use sdram
    sram_used = 0;
    sdram_usable = DAISY_SEED_SDRAM_SIZE;
    sdram_used = 0;
}


//****************************************
//****************************************
//****************************************

t_ptr genlib_sysmem_newptr(t_ptr_size size) 
{
	return (t_ptr)daisy_allocate(size);
}


t_ptr genlib_sysmem_newptrclear(t_ptr_size size) 
{
	t_ptr p = genlib_sysmem_newptr(size);
	if (p) 
    {
        daisy_memset(p, 0, size);
    }
	return p;
}

//****************************************
//****************************************
//****************************************

void genlib_report_error(const char *s)
{
    hw.Print("Err: %s\n", s);
}

void genlib_report_message(const char *s)
{
    hw.Print("Msg: %s\n", s);
}


unsigned long genlib_ticks() 
{ 
	return 0; //daisy::System::GetTick(); 
}


//****************************************
//****************************************
//****************************************



t_ptr genlib_sysmem_resizeptr(void *ptr, t_ptr_size newsize) 
{
	// this function only seems to get called by SineData if reset() happens a second time
	// this shouldn't ever happen, so we default to new allocation:
	return genlib_sysmem_newptr(newsize);
}

void genlib_sysmem_freeptr(void *ptr) 
{
	// nothing to do, we are not running memory management here
}

void genlib_set_zero64(t_sample *memory, long size) 
{
	long i;
	for (i = 0; i < size; i++, memory++) *memory = 0.;
}

// NEED THIS FOR WINDOWS:
void *operator new(size_t size) { return genlib_sysmem_newptr(size); }
void *operator new[](size_t size) { return genlib_sysmem_newptr(size); }
void operator delete(void *p) throw() { genlib_sysmem_freeptr(p); }
void operator delete[](void *p) throw() { genlib_sysmem_freeptr(p); }



void *genlib_obtain_reference_from_string(const char *name) {
	return 0; // to be implemented
}

// the rest is stuff to isolate gensym, attrs, atoms, buffers etc.
t_genlib_buffer *genlib_obtain_buffer_from_reference(void *ref) {
	return 0; // to be implemented
}

t_genlib_err genlib_buffer_edit_begin(t_genlib_buffer *b) {
	return 0; // to be implemented
}

t_genlib_err genlib_buffer_edit_end(t_genlib_buffer *b, long valid) {
	return 0; // to be implemented
}

t_genlib_err genlib_buffer_getinfo(t_genlib_buffer *b, t_genlib_buffer_info *info) {
	return 0; // to be implemented
}

char *genlib_reference_getname(void *ref) {
	return 0; // to be implemented
}

void genlib_buffer_dirty(t_genlib_buffer *b) {
	 // to be implemented
}

t_genlib_err genlib_buffer_perform_begin(t_genlib_buffer *b) {
	return 0; // to be implemented
}
void genlib_buffer_perform_end(t_genlib_buffer *b) {
	// to be implemented
}

t_sample gen_msp_pow(t_sample value, t_sample power)
{
	return pow(value, power);
}

void genlib_data_setbuffer(t_genlib_data *b, void *ref)
{
	genlib_report_error("setbuffer not supported for export targets\n");
}


typedef struct {
	t_genlib_data_info	info;
	t_sample			cursor;	// used by Delay
	//t_symbol *		name;
} t_dsp_gen_data;

t_genlib_data *genlib_obtain_data_from_reference(void *ref) {
	t_dsp_gen_data *self = (t_dsp_gen_data *)genlib_sysmem_newptr(sizeof(t_dsp_gen_data));
	self->info.dim = 0;
	self->info.channels = 0;
	self->info.data = 0;
	self->cursor = 0;
	return (t_genlib_data *)self;
}


t_genlib_err genlib_data_getinfo(t_genlib_data *b, t_genlib_data_info *info) {
	t_dsp_gen_data *self = (t_dsp_gen_data *)b;
	info->dim = self->info.dim;
	info->channels = self->info.channels;
	info->data = self->info.data;
	return GENLIB_ERR_NONE;
}

void genlib_data_release(t_genlib_data *b) {
	t_dsp_gen_data *self = (t_dsp_gen_data *)b;
	if (self->info.data) {
		genlib_sysmem_freeptr(self->info.data);
		self->info.data = 0;
	}
}

long genlib_data_getcursor(t_genlib_data *b) {
	t_dsp_gen_data *self = (t_dsp_gen_data *)b;
	return long(self->cursor);
}

void genlib_data_setcursor(t_genlib_data *b, long cursor) {
	t_dsp_gen_data *self = (t_dsp_gen_data *)b;
	self->cursor = t_sample(cursor);
}

void genlib_data_resize(t_genlib_data *b, long s, long c) {
	t_dsp_gen_data *self = (t_dsp_gen_data *)b;

	size_t sz, oldsz, copysz;
	t_sample *old = 0;
	t_sample *replaced = 0;
	int i, j, copydim, copychannels, olddim, oldchannels;

	//printf("data resize %d %d\n", s, c);

	// cache old for copying:
	old = self->info.data;
	olddim = self->info.dim;
	oldchannels = self->info.channels;

	// limit [data] size:
	if (s * c > DATA_MAXIMUM_ELEMENTS) {
		s = DATA_MAXIMUM_ELEMENTS/c;
		genlib_report_message("warning: constraining [data] to < 256MB");
	}
	// bytes required:
	sz = sizeof(t_sample) * s * c;
	oldsz = sizeof(t_sample) * olddim * oldchannels;

	if (old && sz == oldsz) {
		// no need to re-allocate, just resize
		// careful, audio thread may still be using it:
		if (s > olddim) {
			self->info.channels = c;
			self->info.dim = s;
		} else {
			self->info.dim = s;
			self->info.channels = c;
		}

		genlib_set_zero64(self->info.data, s * c);
		return;

	} else {

		// allocate new:
		replaced = (t_sample *)genlib_sysmem_newptr(sz);

		// check allocation:
		if (replaced == 0) {
			genlib_report_error("data: out of memory");
			// try to reallocate with a default/minimal size instead:
			if (s > 512 || c > 1) {
				genlib_data_resize((t_genlib_data *)self, 512, 1);
			} else {
				// if this fails, then Max is kaput anyway...
				genlib_data_resize((t_genlib_data *)self, 4, 1);
			}
			return;
		}

		// fill with zeroes:
		genlib_set_zero64(replaced, s * c);

		// copy in old data:
		if (old) {
			// frames to copy:
			// clamped:
			copydim = olddim > s ? s : olddim;
			// use memcpy if channels haven't changed:
			if (c == oldchannels) {
				copysz = sizeof(t_sample) * copydim * c;
				//post("reset resize (same channels) %p %p, %d", self->info.data, old, copysz);
				memcpy(replaced, old, copysz);
			} else {
				// memcpy won't work if channels have changed,
				// because data is interleaved.
				// clamp channels copied:
				copychannels = oldchannels > c ? c : oldchannels;
				//post("reset resize (different channels) %p %p, %d %d", self->info.data, old, copydim, copychannels);
				for (i = 0; i < copydim; i++) {
					for (j = 0; j < copychannels; j++) {
						replaced[j + i * c] = old[j + i * oldchannels];
					}
				}
			}
		}

		// now update info:
		if (old == 0) {
			self->info.data = replaced;
			self->info.dim = s;
			self->info.channels = c;
		} else {
			// need to be careful; the audio thread may still be using it
			// since dsp_gen_data is preserved through edits
			// the order of resizing has to be carefully done
			// to prevent indexing out of bounds
			// (or maybe I'm being too paranoid here...)
			if (oldsz > sz) {
				// shrink size first
				if (s > olddim) {
					self->info.channels = c;
					self->info.dim = s;
				} else {
					self->info.dim = s;
					self->info.channels = c;
				}
				self->info.data = replaced;
			} else {
				// shrink size after
				self->info.data = replaced;
				if (s > olddim) {
					self->info.channels = c;
					self->info.dim = s;
				} else {
					self->info.dim = s;
					self->info.channels = c;
				}
			}

			// done with old:
			genlib_sysmem_freeptr(old);

		}
	}
}

void genlib_reset_complete(void *data) 
{

}


size_t genlib_getstatesize(CommonState *cself, getparameter_method getmethod) {
	return 0;
}

short genlib_getstate(CommonState *cself, char *state, getparameter_method getmethod) {
	return 0;
}

short genlib_setstate(CommonState *cself, const char *state, setparameter_method setmethod) {
	return 0;
}

