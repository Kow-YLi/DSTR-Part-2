#ifndef CIRCULAR_QUEUE_HPP
#define CIRCULAR_QUEUE_HPP


template <typename T, int CAPACITY>
class CircularQueue {
private:
    T items[CAPACITY];
    int front;
    int rear;
    int count;

public:
    CircularQueue() : front(0), rear(-1), count(0) {}

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == CAPACITY; }
    int size() const { return count; }
    int capacity() const { return CAPACITY; }

    bool enqueue(const T& item) {
        if (isFull()) {
            return false;
        }

        rear = (rear + 1) % CAPACITY;
        items[rear] = item;
        count++;
        return true;
    }

    bool dequeue() {
        if (isEmpty()) {
            return false;
        }

        front = (front + 1) % CAPACITY;
        count--;
        return true;
    }

    bool peek(T& result) const {
        if (isEmpty()) {
            return false;
        }

        result = items[front];
        return true;
    }


    bool getAt(int position, T& result) const {
        if (position < 0 || position >= count) {
            return false;
        }

        result = items[(front + position) % CAPACITY];
        return true;
    }

   
    int indexOf(int position) const {
        return (front + position) % CAPACITY;
    }
};

#endif 
