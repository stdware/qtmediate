#ifndef QMCHRONOMAP_H
#define QMCHRONOMAP_H

#include <list>
#include <vector>
#include <unordered_map>

template <class K, class T, class H = std::hash<K>>
class QMChronoMap {
private:
    std::list<std::pair<K, T>> m_list;
    std::unordered_map<K, typename decltype(m_list)::iterator, H> m_map;

public:
    typedef T mapped_type;
    typedef K key_type;
    typedef typename decltype(m_list)::difference_type difference_type;
    typedef int size_type;

    QMChronoMap() {
    }

    QMChronoMap(const QMChronoMap &other) {
        for (const auto &item : other.m_list) {
            append(item.first, item.second);
        }
    }

    QMChronoMap(QMChronoMap &&other) noexcept {
        m_list = std::move(other.m_list);
        m_map = std::move(other.m_map);
    }

    QMChronoMap &operator=(const QMChronoMap &other) {
        clear();
        for (const auto &item : other.m_list) {
            append(item.first, item.second);
        }
        return *this;
    }

    QMChronoMap &operator=(QMChronoMap &&other) noexcept {
        m_list = std::move(other.m_list);
        m_map = std::move(other.m_map);
        return *this;
    }

    QMChronoMap(std::initializer_list<std::pair<K, T>> list) {
        for (typename std::initializer_list<std::pair<K, T>>::const_iterator it = list.begin();
             it != list.end(); ++it)
            append(it->first, it->second);
    }

    template <typename InputIterator>
    QMChronoMap(InputIterator f, InputIterator l) {
        for (; f != l; ++f)
            append(f.key(), f.value());
    }

    inline bool operator==(const QMChronoMap &other) const {
        return m_list == other.m_list;
    }

    inline bool operator!=(const QMChronoMap &other) const {
        return m_list != other.m_list;
    }

    class iterator {
    public:
        iterator() = default;

        typedef typename decltype(m_list)::iterator::iterator_category iterator_category;
        typedef typename decltype(m_list)::iterator::difference_type difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;

        inline const K &key() const {
            return i->first;
        }
        inline T &value() const {
            return i->second;
        }
        inline T &operator*() const {
            return value();
        }
        inline T *operator->() const {
            return &value();
        }
        inline bool operator==(const iterator &o) const {
            return i == o.i;
        }
        inline bool operator!=(const iterator &o) const {
            return i != o.i;
        }
        inline iterator &operator++() {
            i++;
            return *this;
        }
        inline iterator operator++(int) {
            iterator r = *this;
            i++;
            return r;
        }
        inline iterator &operator--() {
            i--;
            return *this;
        }
        inline iterator operator--(int) {
            iterator r = *this;
            i--;
            return r;
        }

    private:
        explicit iterator(const typename decltype(m_list)::iterator &i) : i(i) {
        }

        typename decltype(m_list)::iterator i;

        friend class QMChronoMap;
        friend class const_iterator;
    };

    class const_iterator {
    public:
        const_iterator() = default;
        const_iterator(const iterator &it) : i(it.i) {
        }

        typedef typename decltype(m_list)::const_iterator::iterator_category iterator_category;
        typedef typename decltype(m_list)::const_iterator::difference_type difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;

        inline const K &key() const {
            return i->first;
        }
        inline const T &value() const {
            return i->second;
        }
        inline const T &operator*() const {
            return value();
        }
        inline const T *operator->() const {
            return &value();
        }
        inline bool operator==(const const_iterator &o) const {
            return i == o.i;
        }
        inline bool operator!=(const const_iterator &o) const {
            return i != o.i;
        }
        inline const_iterator &operator++() {
            i++;
            return *this;
        }
        inline const_iterator operator++(int) {
            iterator r = *this;
            i++;
            return r;
        }
        inline const_iterator &operator--() {
            i--;
            return *this;
        }
        inline const_iterator operator--(int) {
            iterator r = *this;
            i--;
            return r;
        }

    private:
        explicit const_iterator(const typename decltype(m_list)::const_iterator &i) : i(i) {
        }

        typename decltype(m_list)::const_iterator i;

        friend class QMChronoMap;
    };

    class key_iterator {
        const_iterator i;

    public:
        typedef typename const_iterator::iterator_category iterator_category;
        typedef typename const_iterator::difference_type difference_type;
        typedef K value_type;
        typedef const K *pointer;
        typedef const K &reference;

        key_iterator() = default;
        explicit key_iterator(const_iterator o) : i(o) {
        }

        const K &operator*() const {
            return i.key();
        }
        const K *operator->() const {
            return &i.key();
        }
        bool operator==(key_iterator o) const {
            return i == o.i;
        }
        bool operator!=(key_iterator o) const {
            return i != o.i;
        }

        inline key_iterator &operator++() {
            ++i;
            return *this;
        }
        inline key_iterator operator++(int) {
            return key_iterator(i++);
        }
        inline key_iterator &operator--() {
            --i;
            return *this;
        }
        inline key_iterator operator--(int) {
            return key_iterator(i--);
        }
        const_iterator base() const {
            return i;
        }
    };

    std::pair<iterator, bool> append(const K &key, const T &val, bool replace = true) {
        iterator tmp;
        if (tryReplace(key, val, &tmp, replace)) {
            return {tmp, false};
        }
        auto it = m_list.insert(m_list.end(), std::make_pair(key, val));
        m_map[key] = it;
        return {iterator(it), true};
    }

    std::pair<iterator, bool> prepend(const K &key, const T &val, bool replace = true) {
        iterator tmp;
        if (tryReplace(key, val, &tmp, replace)) {
            return {tmp, false};
        }
        auto it = m_list.insert(m_list.begin(), std::make_pair(key, val));
        m_map[key] = it;
        return {iterator(it), true};
    }

    std::pair<iterator, bool> insert(const const_iterator &it, const K &key, const T &val,
                                     bool replace = true) {
        iterator tmp;
        if (tryReplace(key, val, &tmp, replace)) {
            return {tmp, false};
        }
        auto it2 = m_list.insert(it.i, std::make_pair(key, val));
        m_map[key] = it2;
        return {iterator(it2), true};
    }

    bool remove(const K &key) {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            return false;
        }
        m_list.erase(it->second);
        m_map.erase(it);
        return true;
    }

    iterator erase(const iterator &it) {
        return erase(const_iterator(it));
    }

    iterator erase(const const_iterator &it) {
        auto it2 = m_map.find(it.key());
        if (it2 == m_map.end()) {
            return iterator();
        }
        auto res = std::next(it2->second);
        m_list.erase(it2->second);
        m_map.erase(it2);
        return iterator(res);
    }

    iterator find(const K &key) {
        auto it = m_map.find(key);
        if (it != m_map.end()) {
            return iterator(it->second);
        }
        return end();
    }

    const_iterator find(const K &key) const {
        return constFind(key);
    }

    const_iterator constFind(const K &key) const {
        auto it = m_map.constFind(key);
        if (it != m_map.cend()) {
            return const_iterator(it->second);
        }
        return cend();
    }

    T value(const K &key, const T &defaultValue = T{}) const {
        auto it = m_map.find(key);
        if (it != m_map.end()) {
            return (*it->second).second;
        }
        return defaultValue;
    }

    inline iterator begin() {
        return iterator(m_list.begin());
    }
    inline const_iterator begin() const {
        return const_iterator(m_list.cbegin());
    }
    inline const_iterator cbegin() const {
        return const_iterator(m_list.cbegin());
    }
    inline const_iterator constBegin() const {
        return const_iterator(m_list.cbegin());
    }
    inline iterator end() {
        return iterator(m_list.end());
    }
    inline const_iterator end() const {
        return const_iterator(m_list.cend());
    }
    inline const_iterator cend() const {
        return const_iterator(m_list.cend());
    }
    inline const_iterator constEnd() const {
        return const_iterator(m_list.cend());
    }
    inline key_iterator keyBegin() const {
        return key_iterator(begin());
    }
    inline key_iterator keyEnd() const {
        return key_iterator(end());
    }
    bool contains(const K &key) const {
        return m_map.find(key) != m_map.end();
    }
    int size() const {
        return m_list.size();
    }
    bool isEmpty() const {
        return m_list.empty();
    }
    void clear() {
        m_list.clear();
        m_map.clear();
    }

    std::vector<K> keys() const {
        std::vector<K> res;
        for (const auto &item : qAsConst(m_list)) {
            res.push_back(item.first);
        }
        return res;
    }

    std::vector<T> values() const {
        std::vector<T> res;
        for (const auto &item : qAsConst(m_list)) {
            res.push_back(item.second);
        }
        return res;
    }

    int capacity() const {
        return m_map.capacity();
    }

    void reserve(int size) {
        m_map.reserve(size);
    }

private:
    bool tryReplace(const K &key, const T &val, iterator *it, bool noDryRun) {
        auto it0 = m_map.find(key);
        if (it0 != m_map.end()) {
            if (noDryRun)
                it0->second->second = val;
            *it = iterator(it0->second);
            return true;
        }
        return false;
    }
};

#endif // QMCHRONOMAP_H
