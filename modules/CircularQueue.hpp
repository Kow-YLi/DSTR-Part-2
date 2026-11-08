#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

// ==================================================
// Circular Queue (fixed-size array, FIFO)
// --------------------------------------------------
// - front points to the first element
// - rear points to the last element
// - Indexes wrap around using % CAPACITY, so freed
//   slots at the start of the array are reused and
//   no elements ever need to be shifted.
// - enqueue, dequeue and peek are all O(1).
//
// Used by Module 3 for:
//   1. The collection counter waiting line
//      (limited waiting area = fixed capacity)
//   2. The delivery rider rotation
//      (round-robin: next free rider is at the front,
//       a rider who finishes rejoins at the rear)
// ==================================================

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

    // Add to the rear (wraps around to index 0 when needed)
    bool enqueue(const T& item) {
        if (isFull()) {
            return false;
        }

        rear = (rear + 1) % CAPACITY;
        items[rear] = item;
        count++;
        return true;
    }

    // Remove from the front
    bool dequeue() {
        if (isEmpty()) {
            return false;
        }

        front = (front + 1) % CAPACITY;
        count--;
        return true;
    }

    // Look at the front element without removing it
    bool peek(T& result) const {
        if (isEmpty()) {
            return false;
        }

        result = items[front];
        return true;
    }

    // Get the element at a position from the front
    // (0 = front). Used for displaying the queue.
    bool getAt(int position, T& result) const {
        if (position < 0 || position >= count) {
            return false;
        }

        result = items[(front + position) % CAPACITY];
        return true;
    }

    // Actual array index of a position (for display)
    int indexOf(int position) const {
        return (front + position) % CAPACITY;
    }
};

#endif // CIRCULAR_QUEUE_H
