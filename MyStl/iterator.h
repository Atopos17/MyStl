#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_
#include <cstddef>
#include <iostream>
using namespace std;
namespace MyStl{
    //���ֵ���������
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag:public input_iterator_tag{};
    struct bidirectional_iterator_tag:public forward_iterator_tag{};
    struct random_access_iterator_tag:public bidirectional_iterator_tag{};
    
    //Ϊ�˱���������¼traits,�˻����Ի����Թ��Զ���iterator�̳�֮
    template <class Category,class T,class Distance=std::ptrdiff_t,class Pointer=T*,class Reference=T&>
    struct iterator{
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
    };
    
    //traitsչ����Iterator���������ԡ�ե֭����traits
    template <class Iterator>
    struct iterator_traits{
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::difference_type difference_type;
        typedef typename Iterator::Pointer pointer;
        typedef typedef Iterator::reference reference;
    };
    
    //���ԭ��ָ�루native pointer)����Ƶ�traits��Ƶ�ƫ�ػ��汾
    template <class T>
    struct iterator_traits<T*>{
        using iterator_category=random_access_iterator_tag;
        using value_type=T;
        using difference_type=ptrdiff_t;
        using pointer=T*;
        using reference=T&;
    };

    //
    template <class T>
    struct iterator_traits<const T*>{
        using iterator_category=random_access_iterator_tag;
        using value_type=T;
        using difference_type=ptrdiff_t;
        using pointer=const T*;
        using reference=const T&;
    };
    template <class Iterator>
    using iterator_category_t=typename iterator_traits<Iterator>::value_type;
    template <class Iterator>
    using value_type_t=typename iterator_traits<Iterator>::value_type;
    template <class Iterator>
    using difference_type_t=typename iterator_traits<Iterator>::difference_type;
    template <class Iterator>
    using pointer_t=typename iterator_traits<Iterator>::pointer;
    template <class Iterator>
    using reference_t=typename iterator_traits<Iterator>::reference;
    //�����������distance����
    template <class InputIterator>
    inline difference_type_t<InputIterator> __distance(InputIterator first,InputIterator last,input_iterator_tag){
        difference_type_t<InputIterator> n=0;
        while(first!=last)
        {
            ++first;
            ++n;
        }
        return n;
    }
    template <class RandomAccessIterator>
    inline difference_type_t<RandomAccessIterator> __distance(RandomAccessIterator first,RandomAccessIterator last,random_access_iterator_tag){    
        return last-first;
    }
    template <class InputIterator>
    inline difference_type_t<InputIterator> distance(InputIterator first,InputIterator last)
    {
        return __distance(first,last,iterator_category_t<InputIterator>);
    }

    //�����������advancce����
    template <class InputIterator,class Distance>
    inline void __anvance(InputIterator &i,Distance n,input_iterator_tag)
    {
        while(n--) ++i;
    }
    template <class InputIterator,class Distance>
    inline void __anvance(InputIterator &i,Distance n,bidirectional_iterator_tag)
    {
        if(n>=0)
            while(n--) ++i;
        else
            while(n++) --i;
    }
    template <class InputIterator,class Distance>
    inline void __anvance(InputIterator &i,Distance n,random_access_iterator_tag)
    {
        i+=n;
    }
    template <class InpuIterator,class Distance>
    inline void advance(InputIterator &i,Distance n)
    {
        __advance(i,n,iterator_category_tInputIterator>());
    }
    //���������ֵ�����insert��reverse��stream
    //insert:back_insert_inerator,front_insert_iterator,insert_iterator
    //reverse:reverse_iterator
    //stream:istream_iterator,ostream_iterator,istreambuf_iterator,ostreambuf_iterator
    //������back_insert_iterator
    //�±���������һ�����õ��࣬�ȽϹŹֵ�һ������
    template <class Container>
    class void_insert_iterator{
    };
    template <class Container>
    class back_insert_iterator{  
    protected:
        Container* container;//�ײ�����
    public:
        using iterator_category=output_iterator_tag;
        using value_type=void;
        using difference_type=void;
        using pointer=void;
        using reference=void;
        explicit back_insert_iterator(Container &value):container(value){}
        back_insert_iterator& operator=(const typename Container::value_type& value)
        {
            container->push_back(value);
            return *this;
        }
        //����������������back_insert_iterator��������(�رչ���)
        //�������������صĶ���back_insert_iterator����������ֻ��������ָ��
        back_insert_iterator<Container>& operator*(){return *this;}
        back_insert_iterator<Conatiner>& operator++(){return *this;}
        back_insert_iterator<Container>& operator++(int){return *this;}
    };
    template <class Container>   
    inline back_insert_iterator<Container> back_inserter(Container& x)
    {
    return back_insert_iterator<Container>(x);
    }
    //������back_insert_iterator�ĸ����������������Ƿ���ʹ��back_insert_iterator
    //������front_insert_iterator
    template <class Container>
    class front_insert_iterator{
        protected:
            Container*  container;//�ײ�����
        public:
            using iterator_category=output_iterator_tag;
            using value_type=void;
            using difference_type=void;
            using pointer=void;
            using reference=void;
        public:
            explicit front_insert_iterator(Container& x):container(&x){}
            front_insert_iterator<Container>& operator=(const typename Container::value_type& value){
                container->push_front(value);
                return *this;
            }
            //����������������front_insert_iterator��������(�رչ���)
            //�������������صĶ���front_insert_iterator����������ֻ��������ָ��
            front_insert_iterator &operator*(){return *this;}
            front_insert_iterator &operator++(){return *this;}
            front_insert_iterator &operator++(int){return *this;}
    };
    //������һ�������������������Ƿ���ʹ��insert_iterator
    template <class Container>
    inline front_insert_iterator<Container> front_inserter(Container& x)
    {
        return front_insert_iterator<Container>(x);
    }

    template <class Container>
    class insert_iterator{
    protected:
        Container* container;//�ײ�����
        typename Container::iterator iter;//����λ��
    public:
        using iterator_category=output_iterator_tag;
        using value_type=void;
        using difference_type=void;
        using pointer=void;
        using reference=void;
    public:
        insert_iterator(Container &value,typename Container::iterator i):container(&value),iter(i){}
        insert_iterator &operator=(const typename Container::value_type& value)
        {
            container->insert(iter,value);
            ++iter;
            return *this;
        }
        //����������������back_insert_iterator��������(�رչ���)
        //�������������صĶ���back_insert_iterator����������ֻ��������ָ��
        insert_iterator &operator*(){return *this;}
        insert_iterator &operator++(){return *this;}
        insert_iterator &operator++(int){return *this;}
    };
    //������һ�������������������Ƿ���ʹ��back_insert_iterator
    template <class Container,class Iterator>
    inline insert_iterator<Container> inserter(Container& x,Iterator i)
    {
        return insert_iterator<Container>(x,i);
    }
    //������reverse_iterator
    template <class Iterator>
    class __reverse_iterator{
        template <class It>
        friend bool operator==(const __reverse_iterator<It> &,const __reverse_iterator<It> &);
        template <class It>
        friend bool operator!=(const __reverse_iterator<It> &,const __reverse_iterator<It> &);
    protected:
        Iterator current;
    public:
        using iterator_category=iterator_category_t<Iterator>;
        using value_type=value_type_t<Iterator>;
        using difference_type=difference_type_t<Iterator>:
        using pointer=pointer_t<Iterator>;
        using reference_t<Iterator>;

        using iterator_type=Iterator; //�������������
        using self=__reverse_iterator;//�������������

    public:
        __reverse_iterator(){}
        explicit __reverse_iterator(Iterator value):current(value){}
        __reverse_iterator(const self &value):current(value.current){}
        iterator_type base() conbst {return current;}
        reference operator*() const{
            Iterator temp=current;
            return *--templ;
        }
        pointer operator->() const {return &(operator*());}
        self &operator++(){
            --current;
            return *this;
        }
        self operator++(int){
            self temp=*this;
            --current;
            return temp;
        }
        self operator--(){
            self temp;
            +++current;
            return temp;
        }
        self operator--(int){
            self temp=*this;
            ++currnet;
            return temp;
        }
        self operator+(difference_type n) const { return self(current-n);}
        self &operator+=(difference_type n) const {
            currnet-=n;
            return *this;
        }
        self operator-(difference_type n) const {return self(current+n);}
        self &operator-=(difference_type n) const{
            current+=n;
            return *this;
        }
        reference operator[](difference_type n)const {return *(*this+n);}
        bool operator==(const self &rhs) const { return current==rhs.current;}
        bool operator!=(const self &rhs) const {return !((*this)==rhs);}
    };
    template <class Iterator>
    inline bool operator==(const __reverse_iterator<Iterator> &lhs,const __reverse_iterator<Iterator> &rhs)
    {
       return lhs.operator==(rhs); 
    }

    template <class Iterator>
    inline bool operator!=(const __reverse_iterator<Iterator> &lhs,const __reverse_iterator<Iterator> &rhs)
    {
        return lhs.operator!=(rhs);
    }
    template <class Iterator>
    inline bool operator<(const __reverse_iterator<Iterator> &lhs,const __reverse_iterator<Iterator> &rhs)
    {
        return rhs.base()<lhs.base();
    }
    template <class Iterator>
    inline bool operator>(const __reverse_iterator<Iterator> &lhs,const __reverse_iterator<Iterator> &rhs)
    {
        return rhs<lhs;
    }
    template <class Iterator>
    inline bool operator<=(const __reverse_iterator<Iterator> &lhs,const __reverse_iterator<Iterator> &rhs)
    {
        return !(rhs<lhs);
    }
    template <class Iterator>
    inline bool operator>=(const __reverse_iterator<Iterator> &lhs,const __reverse_iterator<Iterator> &rhs)
    {
        return !(lhs<rhs);
    }
    template <class Iterator>
    inline typename __reverse_iterator<Iterator>::difference_type operator-(const __reverse_iterator<Iterator> &lhs,const __reverse_iterator<Iterator> &rhs)
    {
        return rhs.base()-lsh.base();
    }
    template <class Iterator>
    inline __reverse_iterator<Iterator> operator+(typename __reverse_iterator<Iterator>::difference_type n,const __reverse_iterator<Iterator> &x)
    {
        return __reverse_iterator<Iterator>(x.base()-n);
    }
    //stream::input_iterator,output_iterator
    template <class T,class Distance=ptrdiff_t>
    class istream_iterator{
        //friend bool operator==__STL_NULL_TMPL_ARGS(const istream_iterator<T,Distance>&,const istream_iterator<T,Distance>&);
    protected:
        std::istream* stream;
        T value;
        bool end_marker;
        void read(){
            end_marker=(*stream)?true:false;
            if(end_marker) *stream>>value;
            end_marker=(*stream)?true:false;
        }
    public:
        using iterator_category=input_iterator_tag;
        using value_type=T;
        using difference=Distance;
        using pointer=const T*;
        using reference=const T&;
        
        istream_iterator():stream(&st::cin),end_marker(false){}
        istream_iterator(std::istream& s):stream(&s){read();}
        reference operator*() const {return value;}
        pointer operator->() const {return &(operator*());}
        istream_iterator<T,Distance>& operator++(){
            read();
            return *this;
        }
        istream_iterator<T,Distance> operator++(int){
            istream_iterator<T,Distance> tmp=*this;
            read();
            return tmp;
        }
    };

    template <class T>
    class ostream_iterator{
    protected:
        std::ostream* stream;
        const char* interval;

    public:
        using iterator_category=output_iterator_tag;
        using value_type=void;
        using difference_type=void;
        using pointer=void;
        using reference=void;
    
        ostream_iterator():stream(&std::cout),interval(nullptr){}
        ostream_iterator(std::ostream& s,const char* c):stream(&s),intervsl(c){}
        //ostream_iterator<int> outiter(cout,' ');
        ostream_iterator<T> &operator=(const T& value){
            *stream<<value;
            if(interval) *stream<<interval;
            return *this;
        }
        ostream_iterator<T>& operator*(){return *this;}
        ostream_iterator<T>& operator++(){return *this;}
        ostarem_iterator<T>& operator++(int){return *this;}
    };
}
#endif // MYSTL_ITERATOR_H