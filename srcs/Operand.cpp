#include "Operand.hpp"

template<typename T>
Operand<T>::Operand<T>() { }

template<typename T>
Operand<T>::Operand<T>(Operand<T> const &copy) { *this = copy; }

template<typename T>
Operand<T>::~Operand<T>() { }

template<typename T>
Operand<T> &Operand<T>::operator=(Operand<T> const &copy) {
    if (this != &copy) {
        this->_value = copy._value;
    }
    return *this;
}

template<typename T>
int          Operand<T>::getPrecision(void) const { return 0; }

template<typename T>
eOperandType Operand<T>::getType     (void) const { return Int8; }

template<typename T>
IOperand const *Operand<T>::operator+(IOperand const &rhs) const { return new IOperand; }

template<typename T>
IOperand const *Operand<T>::operator-(IOperand const &rhs) const { return new IOperand; }

template<typename T>
IOperand const *Operand<T>::operator*(IOperand const &rhs) const { return new IOperand; }

template<typename T>
IOperand const *Operand<T>::operator/(IOperand const &rhs) const { return new IOperand; }

template<typename T>
IOperand const *Operand<T>::operator%(IOperand const &rhs) const { return new IOperand; }
