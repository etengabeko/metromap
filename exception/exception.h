#ifndef METROMAP_EXCEPTION_EXCEPTION_H
#define METROMAP_EXCEPTION_EXCEPTION_H

#include <QString>
#include <stdexcept>

namespace metro
{

class Exception : public std::logic_error
{
public :
  Exception(const QString& cause) :
    std::logic_error(cause.toStdString())
  {}

  virtual ~Exception() throw()
  {}

};

} // metro

#endif // METROMAP_EXCEPTION_EXCEPTION_H
