#include <cstring>
#include "br_predictor.h"

// Branch predictor
br_predictor_t::br_predictor_t(unsigned m_hist_len) :
    bhr(0),
    pht(0),
    h(m_hist_len) {
    // Create a pattern history table (PHT).
    pht = new uint8_t[1 << h];
    for(uint64_t i = 0; i < sizeof(pht); i++) pht[i] = 0;  // initialize pht array
}

br_predictor_t::~br_predictor_t() {
    // Deallocate the PHT.
    delete [] pht;
}

// Is a branch predicted to be taken?
bool br_predictor_t::is_taken(inst_t *m_inst) {
    pht_index = (bhr^(m_inst->pc>>2))&0xff;
    m_inst->pht_index_tmp = pht_index;
    if(pht[pht_index] >= 2) return true;  // return true if counter value is bigger than 2
    else return false;  // return false if not
}

// Update a prediction counter.
void br_predictor_t::update(inst_t *m_inst) {
    bhr <<= 1;  // shift bhr to left
    bhr |= m_inst->branch_taken & 1;  // record the lowest bit of branch state
    if(pht[m_inst->pht_index_tmp] != 3) pht[m_inst->pht_index_tmp]++;  // update pht
}



// Branch target buffer
br_target_buffer_t::br_target_buffer_t(uint64_t m_size) :
    num_entries(m_size),
    buffer(0) {
    // Create a direct-mapped branch target buffer (BTB).
    buffer = new uint64_t[num_entries];
    for(uint64_t i = 0; i < sizeof(buffer); i++) buffer[i] = 0;  // initialize buffer
}

br_target_buffer_t::~br_target_buffer_t() {
    // Deallocate the target address array.
    delete [] buffer;
}

// Get a branch target address.
uint64_t br_target_buffer_t::get_target(uint64_t m_pc) {
    // Always return branch target address.
    return buffer[(m_pc>>2) & 0xf];
}

// Update the branch target buffer.
void br_target_buffer_t::update(uint64_t m_pc, uint64_t m_target_addr) {
    buffer[(m_pc>>2) & 0xf] = m_target_addr;
}

