// Took from https://embeddedartistry.com/blog/2017/4/6/circular-buffers-in-cc
#include <cstdio>

#include <memory>
#include <mutex>

namespace GGB::Hardware::Audio
{
    template <class T>
    class RingBuffer 
    {
        public:
            explicit RingBuffer(size_t size) :
                buf_(std::unique_ptr<T[]>(new T[size])),
                max_size_(size)
            { }

            void put(T item)
            {
                std::lock_guard<std::mutex> lock(mutex_);

                buf_[head_] = item;

                if(full_)
                {
                    tail_ = (tail_ + 1) % max_size_;
                }

                head_ = (head_ + 1) % max_size_;

                full_ = head_ == tail_;
            }

            T get()
            {
                std::lock_guard<std::mutex> lock(mutex_);

                if(empty())
                {
                    return T();
                }

                //Read data and advance the tail (we now have a free space)
                auto val = buf_[tail_];
                full_ = false;
                tail_ = (tail_ + 1) % max_size_;

                return val;
            }

            void reset()
            {
                std::lock_guard<std::mutex> lock(mutex_);
                head_ = tail_;
                full_ = false;
            }

            bool empty() const
            {
                //if head and tail are equal, we are empty
                return (!full_ && (head_ == tail_));
            }

            bool full() const
            {
                //If tail is ahead the head by 1, we are full
                return full_;
            }

            size_t capacity() const
            {
                return max_size_;
            }

            size_t size() const
            {
                size_t size = max_size_;

                if(!full_)
                {
                    if(head_ >= tail_)
                    {
                        size = head_ - tail_;
                    }
                    else
                    {
                        size = max_size_ + head_ - tail_;
                    }
                }

                return size;
            }

        private:
            std::mutex mutex_;
            std::unique_ptr<T[]> buf_;
            size_t head_ = 0;
            size_t tail_ = 0;
            const size_t max_size_;
            bool full_ = 0;
    };
}