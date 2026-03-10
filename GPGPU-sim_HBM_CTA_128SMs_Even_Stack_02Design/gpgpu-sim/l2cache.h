// Copyright (c) 2009-2011, Tor M. Aamodt
// The University of British Columbia
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
// Neither the name of The University of British Columbia nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef MC_PARTITION_INCLUDED
#define MC_PARTITION_INCLUDED

//#include "dram.h"
#include "../abstract_hardware_model.h"
#include "../ramulator_sim/gpu_wrapper.h"

#include <list>
#include <queue>
#include <zlib.h>
#include <set>

#include "../../launcher/mk-sched/mk_scheduler.h"

#include <unordered_map>
#include <vector>

class coalescer_table{

private:
    std::unordered_map<int, std::vector<mem_fetch*>> table;
    size_t max_rows ;
    size_t max_mfs_per_row ;
    size_t remote_request_num;
public:
    coalescer_table(size_t max_rows, size_t max_mfs_per_row)
        : max_rows(max_rows), max_mfs_per_row(max_mfs_per_row),remote_request_num(0) {}

    bool has_row(int addr) const {
        return table.find(addr) != table.end();
    }

    bool is_row_full(int addr) const {
        assert(has_row(addr));
        return  table.at(addr).size() >= max_mfs_per_row;
    }

    bool is_table_full() const {
        return table.size() >= max_rows;
    }

    void add_mf_to_row(int addr, mem_fetch* mf) {
        assert(!is_row_full(addr));
        int old_row_num =  get_mf_count_in_row(addr);
      //  printf("reuest: the coalescer-old_row_num is %u \n", old_row_num);
        table[addr].push_back(mf);
        int new_row_num =  get_mf_count_in_row(addr);
       // printf("reuest: the coalescer_new_row_num is %u \n", new_row_num);
       if( new_row_num = old_row_num +1)
           printf("reuest: the coalescer_new_row_num is ture \n");
        else
          printf("reuest: the coalescer_new_row_num is failed \n");
    }

    void add_row(int addr, mem_fetch* mf) {
        assert(!is_table_full() && !has_row(addr));
        int old_table_num =  get_current_row_count();
     //   printf("reuest: the coalescer-old_table_num is %u \n", old_table_num);

        table[addr] = std::vector<mem_fetch*>{mf};
        int new_table_num =  get_current_row_count();
       // printf("reuest: the coalescer-new_table_num is %u \n", new_table_num);
        if( new_table_num = old_table_num +1)
            printf("reuest: the coalescer_new_table_num is ture \n");
        else
            printf("reuest: the coalescer_new_table_num is failed  \n");
    }
   
    //Find all mfs by addr 
    std::vector<mem_fetch*> retrieve_mfs(int addr) {
        assert(has_row(addr));
        std::vector<mem_fetch*> retrieved_mfs = table[addr];
        return retrieved_mfs;
  }

   mem_fetch* get_first_mf_in_row(int addr) const {
    assert(has_row(addr));
    const std::vector<mem_fetch*>& mfs = table.at(addr);
    assert(!mfs.empty());
    return mfs[0];
}

    void remove_row(int addr) {
        assert(has_row(addr));
        table.erase(addr);
    }

    size_t get_current_row_count() const {
        return table.size();
    }

    size_t get_mf_count_in_row(int addr) const {
        assert(has_row(addr));
        return table.at(addr).size();
    }

    size_t getCounter() const {
        return remote_request_num;
    }

    void incrementCounter() {
        remote_request_num++;
    }
};

extern bool KAIN_NoC_r_Request_Remote_full[4];

class KAIN_GPU_chiplet 
{
    public:
        KAIN_GPU_chiplet(unsigned MC_count)
        {
            assert(MC_count<4);
//            printf("KKKK\n");
//            fflush(stdout);

            for(int i = 0; i < 128; i++)
            {
               Request[i] = new fifo_pipeline<mem_fetch>("Request_",0,256);  
               Reply[i] = new fifo_pipeline<mem_fetch>("Reply_",0,256);  
            }
            for(int i = 0; i < 4; i++)
            {
               //Request_Near[i] = new fifo_pipeline<mem_fetch>("Request_Near_",0,256);  
               Request_Remote[i] = new fifo_pipeline<mem_fetch>("Request_Remote_",0,256);  

              // Reply_Near[i] = new fifo_pipeline<mem_fetch>("Reply_Near_",0,256);  
               Reply_Remote[i] = new fifo_pipeline<mem_fetch>("Reply_Remote_",0,256);  

               Request_turn[i] = 0;
               Reply_turn[i] = 0;
               Last_Remote_ID_ID[i] = 0;
               Last_Near_ID_ID[i] = 0;


                for(int j = 0; j < 4; j++)
                {
                //    Request_Remote_Src_From[i][j] = new fifo_pipeline<mem_fetch>("Request_Remote_",0,256);
                //    Reply_Remote_Src_From[i][j] = new fifo_pipeline<mem_fetch>("Reply_Remote_",0,256);
                //    Request_Near_Src_From[i][j] = new fifo_pipeline<mem_fetch>("Request_Near_",0,256);
                //    Reply_Near_Src_From[i][j] = new fifo_pipeline<mem_fetch>("Reply_Near_",0,256);
                //    Remote_Request_turn[i][j] = 0;
                //    Near_Request_turn[i][j] = 0;
                } 

            }

            LastTime_Request = 0;
            LastTime_Reply = 0;
            Remote_cycle = 0;
            Near_cycle = 0;
            Last_Remote_ID = 0;
//            printf("KKKK2\n");
//            fflush(stdout);
        }
        void Chiplet_cycle_near()//Near bandwidth is 128byte * Operating frequency
        {
            for(int i = 0; i < 4; i++)
            {
                 for(int jj = 0; jj < 4; jj++)
                 {
                    if(!Request_Remote[i]->empty())
                    {
                        mem_fetch *mf = Request_Remote[i]->top();
                        if(!Request[mf->get_sub_partition_id()]->full())
                        {
                            Request[mf->get_sub_partition_id()]->push(mf);
                            KAIN_request_Remote++;
                            //printf("mf id %u, TPC %d, mid %d, Write %d\n", mf->get_request_uid(),mf->get_tpc(), mf->get_chip_id(), mf->get_is_write());
                            //fflush(stdout);
                            Request_Remote[i]->pop();
                            KAIN_NoC_r_Request_Remote_full[i] = Request_Remote[i]->full();
                        }
                    }
                  }


                    if(!Reply_Remote[i]->empty())
                    {
                        mem_fetch *mf = Reply_Remote[i]->top();
                        if(!Reply[mf->get_tpc()]->full())
                        {
                            Reply[mf->get_tpc()]->push(mf);
                            KAIN_reply_Remote++;
                            Reply_Remote[i]->pop();
                        }
                    }

            }

        }







        bool Request_Remote_full(unsigned HBM_stack_id)
        {
              return Request_Remote[HBM_stack_id]->full(); 
        }

        void Request_Remote_push(mem_fetch *mf, unsigned HBM_stack_id)
        {
              Request_Remote[HBM_stack_id]->push(mf); 
              KAIN_NoC_r_Request_Remote_full[HBM_stack_id] = Request_Remote[HBM_stack_id]->full();
        }

        bool Reply_Remote_full(unsigned HBM_stack_id)
        {
              return Reply_Remote[HBM_stack_id]->full(); 
        }

        void Reply_Remote_push(mem_fetch *mf, unsigned HBM_stack_id)
        {
              return Reply_Remote[HBM_stack_id]->push(mf); 
        }




        bool request_empty(unsigned m_id)
        {
            return Request[m_id]->empty(); 
        }

        mem_fetch * request_top(unsigned m_id)
        {
            return Request[m_id]->top();
        }

        mem_fetch * request_pop_front(unsigned m_id)
        {
            return Request[m_id]->pop();
        }



        mem_fetch * reply_top(unsigned m_id)
        {
            return Reply[m_id]->top();
        }

        mem_fetch * reply_pop_front(unsigned m_id)
        {
            return Reply[m_id]->pop();
        }



    private:
        fifo_pipeline<mem_fetch> *Request_Near[4];
        fifo_pipeline<mem_fetch> *Request_Remote[4];
        fifo_pipeline<mem_fetch> *Request[128];
        fifo_pipeline<mem_fetch> *Reply_Near[4];
        fifo_pipeline<mem_fetch> *Reply_Remote[4];
        fifo_pipeline<mem_fetch> *Reply[128];

        fifo_pipeline<mem_fetch> *Request_Remote_Src_From[4][4];//8 is Src, 8 is from
        fifo_pipeline<mem_fetch> *Reply_Remote_Src_From[4][4];

        fifo_pipeline<mem_fetch> *Request_Near_Src_From[4][4];//8 is Src, 8 is from
        fifo_pipeline<mem_fetch> *Reply_Near_Src_From[4][4];



unsigned long long KAIN_request_Near;
unsigned long long KAIN_request_Remote;
unsigned long long KAIN_reply_Near;
unsigned long long KAIN_reply_Remote;

        int LastTime_Request;
        int LastTime_Reply;
        int Request_turn[4];
        int Reply_turn[4];
        long long Remote_cycle;
        long long Near_cycle;
        int Remote_Request_turn[4][4];
        int Near_Request_turn[4][4];
        int Last_Remote_ID_ID[4];
        int Last_Near_ID_ID[4];
        int Last_Remote_ID;
};


class mem_fetch;

class partition_mf_allocator : public mem_fetch_allocator {
public:
    partition_mf_allocator( const memory_config *config )
    {
        m_memory_config = config;
    }
    virtual mem_fetch * alloc(const class warp_inst_t &inst, const mem_access_t &access) const 
    {
        abort();
        return NULL;
    }
    virtual mem_fetch * alloc(new_addr_type addr, mem_access_type type, unsigned size, bool wr) const;
private:
    const memory_config *m_memory_config;
};

// Memory partition unit contains all the units assolcated with a single DRAM channel. 
// - It arbitrates the DRAM channel among multiple sub partitions.  
// - It does not connect directly with the interconnection network. 
class memory_partition_unit
{
public: 
   memory_partition_unit( unsigned partition_id, const struct memory_config *config, class memory_stats_t *stats );
   ~memory_partition_unit(); 

   bool busy() const;

   void cache_cycle( unsigned cycle );
   void dram_cycle();

   void set_done( mem_fetch *mf );

   void visualizer_print( gzFile visualizer_file ) const;
   void print_stat( FILE *fp ) const;
  // { m_dram->print_stat(fp); }
//   void visualize() const { m_dram->visualize(); }
   void print( FILE *fp ) const;
   void print_kain()
   {   
   		assert(0);
        //m_dram->print_kain(); 
   }  
   float KAIN_app1_bw_util()
   {
   		assert(0);//Need to be implemented in HBM
        //return m_dram->KAIN_app1_bw_util(); 
   }
   float KAIN_app2_bw_util()
   {
   		assert(0);//Need to be implemented in HBM
        //return m_dram->KAIN_app2_bw_util(); 
   }

   class memory_sub_partition * get_sub_partition(int sub_partition_id) 
   {
      return m_sub_partition[sub_partition_id]; 
   }

   // Power model
   void set_dram_power_stats(unsigned &n_cmd,
                             unsigned &n_activity,
                             unsigned &n_nop,
                             unsigned &n_act,
                             unsigned &n_pre,
                             unsigned &n_rd,
                             unsigned &n_wr,
                             unsigned &n_req) const;

   int global_sub_partition_id_to_local_id(int global_sub_partition_id) const; 

   unsigned get_mpid() const { return m_id; }

private: 

   unsigned m_id;
   const struct memory_config *m_config;
   class memory_stats_t *m_stats;
   class memory_sub_partition **m_sub_partition; 
//   class dram_t *m_dram;
	class GpuWrapper *m_dram_r;
   class arbitration_metadata
   {
   public: 
      arbitration_metadata(const struct memory_config *config); 

      // check if a subpartition still has credit 
      bool has_credits(int inner_sub_partition_id) const; 
      // borrow a credit for a subpartition 
      void borrow_credit(int inner_sub_partition_id); 
      // return a credit from a subpartition 
      void return_credit(int inner_sub_partition_id); 

      // return the last subpartition that borrowed credit 
      int last_borrower() const { return m_last_borrower; } 

      void print( FILE *fp ) const; 
   private: 
      // id of the last subpartition that borrowed credit 
      int m_last_borrower; 

      int m_shared_credit_limit; 
      int m_private_credit_limit; 

      // credits borrowed by the subpartitions
      std::vector<int> m_private_credit; 
      int m_shared_credit; 
   }; 
   arbitration_metadata m_arbitration_metadata; 

   // determine wheither a given subpartition can issue to DRAM 
   bool can_issue_to_dram(int inner_sub_partition_id); 

   // model DRAM access scheduler latency (fixed latency between L2 and DRAM)
   struct dram_delay_t
   {
      unsigned long long ready_cycle;
      class mem_fetch* req;
   };
   std::list<dram_delay_t> m_dram_latency_queue;
};

class memory_sub_partition
{
public:
   memory_sub_partition( unsigned sub_partition_id, const struct memory_config *config, class memory_stats_t *stats );
   ~memory_sub_partition(); 

   unsigned get_id() const { return m_id; } 

   bool busy() const;

   void cache_cycle( unsigned cycle );

   bool full() const;
   void push( class mem_fetch* mf, unsigned long long clock_cycle );
   class mem_fetch* pop(); 
   class mem_fetch* top();
   void set_done( mem_fetch *mf );

   unsigned flushL2();

   // interface to L2_dram_queue
   bool L2_dram_queue_empty() const; 
   class mem_fetch* L2_dram_queue_top() const; 
   void L2_dram_queue_pop(); 

   // interface to dram_L2_queue
   bool dram_L2_queue_full() const; 
   void dram_L2_queue_push( class mem_fetch* mf ); 

   void visualizer_print( gzFile visualizer_file );
   void print_cache_stat(unsigned &accesses, unsigned &misses) const;
   void print( FILE *fp ) const;

   void accumulate_L2cache_stats(class cache_stats &l2_stats) const;
   void get_L2cache_sub_stats(struct cache_sub_stats &css) const;
   void get_L2cache_sub_stats_kain(unsigned cluster_id, struct cache_sub_stats &css) const;
   void clear_L2cache_sub_stats_kain();

private:
// data
   unsigned m_id;  //< the global sub partition ID
   const struct memory_config *m_config;
   class l2_cache *m_L2cache;
   class L2interface *m_L2interface;
   partition_mf_allocator *m_mf_allocator;

   // model delay of ROP units with a fixed latency
   struct rop_delay_t
   {
    	unsigned long long ready_cycle;
    	class mem_fetch* req;
   };
   std::queue<rop_delay_t> m_rop;

   // these are various FIFOs between units within a memory partition
   fifo_pipeline<mem_fetch> *m_icnt_L2_queue;
   fifo_pipeline<mem_fetch> *m_L2_dram_queue;
   fifo_pipeline<mem_fetch> *m_dram_L2_queue;
   fifo_pipeline<mem_fetch> *m_L2_icnt_queue; // L2 cache hit response queue

   class mem_fetch *L2dramout; 
   unsigned long long int wb_addr;

   class memory_stats_t *m_stats;

   std::set<mem_fetch*> m_request_tracker;

   friend class L2interface;

public:
   void set_mk_scheduler(MKScheduler* mk_sched);
};

class L2interface : public mem_fetch_interface {
public:
    L2interface( memory_sub_partition *unit ) { m_unit=unit; }
    virtual ~L2interface() {}
    virtual bool full(mem_fetch *mf, unsigned size, bool write) const 
    {
        // assume read and write packets all same size
        return m_unit->m_L2_dram_queue->full();
    }
    virtual void push(mem_fetch *mf) 
    {
        mf->set_status(IN_PARTITION_L2_TO_DRAM_QUEUE,0/*FIXME*/);
        m_unit->m_L2_dram_queue->push(mf);
    }
private:
    memory_sub_partition *m_unit;
};

#endif
