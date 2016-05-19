template <typename T>
SyncQueue<T>::SyncQueue(std::memory_order order): order(order), mtx(order) {

}

template <typename T>
bool SyncQueue<T>::empty() const {
    std::lock_guard<MutexT> lg(mtx);
    return data.empty();
}

template <typename T>
size_t SyncQueue<T>::size() const {
    std::lock_guard<MutexT> lg(mtx);
    return data.size();
}

template <typename T>
T SyncQueue<T>::front() const {
    std::lock_guard<MutexT> lg(mtx);
    return data.front();
}
template <typename T>
T SyncQueue<T>::back() const {
    std::lock_guard<MutexT> lg(mtx);
    return data.back();
}
template <typename T>
void SyncQueue<T>::push(ConstReference x) {
    std::lock_guard<MutexT> lg(mtx);
    data.push_back(x);
    cond.notify_one();
}
template <typename T>
void SyncQueue<T>::pop() {
    std::lock_guard<MutexT> lg(mtx);
    if (data.empty())
        throw std::range_error("Popping element from empty SyncQueue");
    data.pop_front();
}
template <typename T>
void SyncQueue<T>::pop_and_wait() {
    std::unique_lock<MutexT> lock(mtx);
//    cond.wait(lock, [&]() { return !data.empty(); } );
    data.pop_front();
}

