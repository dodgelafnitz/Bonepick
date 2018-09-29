#ifndef ENGINE_UTILITY_CONTAINERS_EXTERNAL_H
#define ENGINE_UTILITY_CONTAINERS_EXTERNAL_H

#include "engine/Utility/Containers/Optional.h"
#include "engine/Utility/Debug.h"
#include "engine/Utility/TemplateTools.h"

//##############################################################################
template <typename T>
struct IExternal;
template <typename T>
class External;
template <typename T>
class WeakExternal;
template <typename T>
class ExternalData;

//##############################################################################
template <typename T>
struct IExternal
{
public:
  virtual ~IExternal(void) {}

  virtual T *       Ptr(void)       = 0;
  virtual T const * Ptr(void) const = 0;
};

//##############################################################################
template <typename T>
class External : public IExternal<T>
{
public:
  External(void) = default;
  External(External const & external);
  External(External && external);

  template <typename... Params, typename = std::enable_if_t<
    !TypeIsDecayedTypes<External<T>, Params...>::value &&
    !TypeIsDecayedTypes<WeakExternal<T>, Params...>::value>>
  External(Params && ... params);

  virtual ~External(void) override;

  T & operator *(void);
  T const & operator *(void) const;

  T * operator ->(void);
  T const * operator ->(void) const;

  External & operator =(External const & external);
  External & operator =(External && external);

  template <typename ... Params>
  T & Emplace(Params && ... params);

  virtual T * Ptr(void) override;
  virtual T const * Ptr(void) const override;

  void Clear(void);

private:
  ExternalData<T> * data_ = nullptr;
};


//##############################################################################
template <typename T>
class ExternalData
{
public:
  static ExternalData * Create(void);

  template <typename ... Params>
  static ExternalData * Create(Params && ... params);

  static void Destroy(ExternalData * data);

  void AddStrongRef(void);
  void RemoveStrongRef(void);

  T *       Ptr(void);
  T const * Ptr(void) const;

  int TotalRefs(void) const;
  bool IsUnique(void) const;

  template <typename ... Params>
  T & Emplace(Params && ... params);

private:
  ExternalData(void) = default;

  template <typename ... Params>
  ExternalData(Params && ... params);

  int        strongRefCount_ = 1;
  Optional<T> value_;
};

//##############################################################################
template <typename T>
External<T>::External(External const & external) :
  data_(external.data_)
{
  external.data_->AddStrongRef();
}

//##############################################################################
template <typename T>
External<T>::External(External && external) :
  data_(external.data_)
{
  external.data_ = nullptr;
}

//##############################################################################
template <typename T>
template <typename ... Params, typename>
External<T>::External(Params && ... params) :
  data_(ExternalData<T>::Create(std::forward<Params &&>(params)...))
{}

//##############################################################################
template <typename T>
External<T>::~External(void)
{
  Clear();
}

//##############################################################################
template <typename T>
T & External<T>::operator *(void)
{
  ASSERT(data_);
  ASSERT(data_->Ptr());

  return *data_->Ptr();
}

//##############################################################################
template <typename T>
T const & External<T>::operator *(void) const
{
  ASSERT(data_);
  ASSERT(data_->Ptr());

  return *data_->Ptr();
}

//##############################################################################
template <typename T>
T * External<T>::operator ->(void)
{
  ASSERT(data_);
  ASSERT(data_->Ptr());

  return data_->Ptr();
}

//##############################################################################
template <typename T>
T const * External<T>::operator ->(void) const
{
  ASSERT(data_);
  ASSERT(data_->Ptr());

  return data_->Ptr();
}

//##############################################################################
template <typename T>
External<T> & External<T>::operator =(External const & external)
{
  if (this == &external)
    return *this;

  if (data_ == external.data_)
    return *this;

  Clear();
  data_ = external.data_;
  data_->AddStrongRef();

  return *this;
}

//##############################################################################
template <typename T>
External<T> & External<T>::operator =(External && external)
{
  if (this == &external)
    return *this;

  Clear();
  data_          = external.data_;
  external.data_ = nullptr;

  return *this;
}

//##############################################################################
template <typename T>
template <typename ... Params>
T & External<T>::Emplace(Params && ... params)
{
  if (data_ && data_->IsUnique())
    data_->Emplace(std::forward<Params &&>(params)...);
  else
  {
    Clear();
    data_ = ExternalData<T>::Create(std::forward<Params &&>(params)...);
  }

  ASSERT(data_);
  ASSERT(data_->Ptr());

  return *data_->Ptr();
}

//##############################################################################
template <typename T>
T * External<T>::Ptr(void)
{
  return data_ ? data_->Ptr() : nullptr;
}

//##############################################################################
template <typename T>
T const * External<T>::Ptr(void) const
{
  return data_ ? data_->Ptr() : nullptr;
}

//##############################################################################
template <typename T>
void External<T>::Clear(void)
{
  if (data_)
  {
    data_->RemoveStrongRef();
    data_ = nullptr;
  }
}

//##############################################################################
template <typename T>
ExternalData<T> * ExternalData<T>::Create(void)
{
  return new ExternalData<T>();
}

//##############################################################################
template <typename T>
template <typename ... Params>
ExternalData<T> * ExternalData<T>::Create(Params && ... params)
{
  return new ExternalData<T>(std::forward<Params &&>(params)...);
}

//##############################################################################
template <typename T>
void ExternalData<T>::Destroy(ExternalData * data)
{
  delete(data);
}

//##############################################################################
template <typename T>
template <typename ... Params>
ExternalData<T>::ExternalData(Params && ... params) :
  strongRefCount_(1),
  value_(std::forward<Params &&>(params)...)
{}

//##############################################################################
template <typename T>
void ExternalData<T>::AddStrongRef(void)
{
  ++strongRefCount_;
}

//##############################################################################
template <typename T>
void ExternalData<T>::RemoveStrongRef(void)
{
  if (--strongRefCount_ == 0)
    Destroy(this);
}

//##############################################################################
template <typename T>
T * ExternalData<T>::Ptr(void)
{
  return value_.Ptr();
}

//##############################################################################
template <typename T>
T const * ExternalData<T>::Ptr(void) const
{
  return value_.Ptr();
}

//##############################################################################
template <typename T>
int ExternalData<T>::TotalRefs(void) const
{
  return strongRefCount_;
}

//##############################################################################
template <typename T>
bool ExternalData<T>::IsUnique(void) const
{
  return TotalRefs() == 1;
}

//##############################################################################
template <typename T>
template <typename ... Params>
T & ExternalData<T>::Emplace(Params && ... params)
{
  return value_.Emplace(std::forward<Params &&>(params)...);
}

#endif
