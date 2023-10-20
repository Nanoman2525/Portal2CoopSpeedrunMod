#ifndef SINGLETON_HPP
#define SINGLETON_HPP

template<typename Derived> class Singleton {
public:
    static Derived* Instance() {
        static Derived instance;
        return &instance;
    }

private:
    Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    friend Derived;
};

#endif // SINGLETON_HPP
