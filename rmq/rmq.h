#ifndef _RMQ_H_
#define _RMQ_H_

#include <vector>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <math.h>


template<class T>
class RMQ
{
    protected:
        const std::vector<T> &A;
        size_t n;

    public:
        RMQ(const std::vector<T> &A) : A(A), n(A.size()) {};

        virtual T operator()(size_t, size_t) const = 0;
};


template<class T>
class NaiveRMQ : public RMQ<T>
{
    public:
        NaiveRMQ(const std::vector<T> &A) : RMQ<T>(A) {};

        virtual T operator()(size_t i, size_t j) const
        {
            assert(i <= j && j < this->n);
            
            T min = this->A[i];
            for(size_t k = i+1; k <= j; ++k)
                min = std::min(min, this->A[k]);

            return min; 
        };
};


template<class T>
class FullyPrecomputedRMQ : public RMQ<T>
{
    private:
        std::vector<std::vector<T> > mins;

    public:
        FullyPrecomputedRMQ(const std::vector<T> &A) : RMQ<T>(A)
        {
            this->mins.resize(this->n);
            for(size_t i = 0; i < this->n; ++i)
            {
                this->mins[i].resize(this->n - i);
                T current_min = this->A[i];
                for(size_t j = i; j < this->n; ++j)
                {
                    current_min = std::min(current_min, this->A[j]);
                    this->mins[i][j-i] = current_min; 
                }
            }
        };

        virtual T operator()(size_t i, size_t j) const
        {
            assert(i <= j && j < this->n);
            
            return this->mins[i][j-i];
        };
};


template<class T>
class BlockRMQ : public RMQ<T>
{
    #define BLOCK_SIZE   (size_t)floor(sqrt(this->n))
    #define NUM_BLOCKS   ceil(this->n / (double)BLOCK_SIZE)
    #define idx(i,j)     (i*BLOCK_SIZE + j)
    #define block_idx(i) (i / BLOCK_SIZE)
    #define offset(i)    (i % BLOCK_SIZE)

    private:
        std::vector<T> block_mins;
        size_t block_size;

        T min_on_block(size_t i, size_t from=0, ssize_t to=-1) const
        {
            T current_min = this->A[idx(i,from)];
            to = to >= 0 ? to : this->block_size;
            for(size_t j = from+1; j < to; ++j)
                current_min = std::min(current_min, this->A[idx(i,j)]);
            return current_min;
        };

    public:
        BlockRMQ(const std::vector<T> &A) : RMQ<T>(A), block_size(BLOCK_SIZE)
        {
            this->block_mins.resize(NUM_BLOCKS);
            for(size_t i = 0; i < NUM_BLOCKS; ++i)
                this->block_mins[i] = this->min_on_block(i);
        };

        virtual T operator()(size_t i, size_t j) const
        {
            assert(i <= j && j < this->n);
            
            size_t i_block = block_idx(i),
                   j_block = block_idx(j);

            T left_block_min = this->min_on_block(i_block, offset(i), this->block_size),
              right_block_min = this->min_on_block(j_block, 0, offset(j)),
              current_min = std::min(left_block_min, right_block_min);
            
            for(size_t k = i_block+1; k < j_block; ++k)
                 current_min = std::min(current_min, this->block_mins[k]);

            return current_min;
        };
};


template<class T>
class SparseTableRMQ : public RMQ<T>
{
    #define log_interval(i,j) (this->logs[j-i+1])
    
    private:
        std::vector<std::vector<T> > mins;
        std::vector<int> logs;

        void compute_logs()
        {
            this->logs.resize(this->n+2);
            int current_log = -1, current_pow = 1;
            for(size_t i = 1; i <= 1+this->n; ++i)
            {
                if(i == current_pow)
                {
                    current_log++;
                    current_pow <<= 1;
                }
                this->logs[i] = current_log;
            }
        };
        
    public:
        SparseTableRMQ(const std::vector<T> &A) : RMQ<T>(A)
        {
            this->mins.resize(this->n);
            this->compute_logs();
            
            for(size_t i = 0; i < this->n; ++i)
            {
                this->mins[i].resize(log_interval(i, this->n));
                this->mins[i][0] = this->A[i];
            }

            for(size_t j = 1; (1 << j) < this->n; ++j)
                for(size_t i = 0; (i + (1 << j) - 1) < this->n; ++i)
                    this->mins[i][j] = std::min(this->mins[i][j-1],
                                                this->mins[i + (1 << (j-1))][j-1]);
        };

        virtual T operator()(size_t i, size_t j) const
        {
            assert(i <= j && j < this->n);
            
            size_t k = log_interval(i, j);
            return std::min(this->mins[i][k],
                            this->mins[j - (1 << k) + 1][k]);
        };
};

#endif
