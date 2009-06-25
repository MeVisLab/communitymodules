// **InsertLicense** code
//-----------------------------------------------------------------------------------
//!
//! Doxygen reference for the MeVisLab Community Sources.
//!\file      mlCommunitySourcesDoc.h

/*! \mainpage MeVisLab Community Sources

This is the Doxygen reference page for the MeVisLab Community Sources.
It contains source code documentation automatically generated on the
Build Server for the Community Sources.

To keep the file and class references up to date please assure that the
file and class comments are added to your source codes and that they are
activated with an "!" at the beginning of comments.

Activating comments for classes, structs, functions, typdefs, enumerators
etc. of course is also highly recommended, because this will also be added
to the generated documentation then.

This is an example demonstrating many commonly used formatting commands and
a full class documentation (Java Script or C++):
\verbatim
//! File containing the ML module class Dinner prepare a dinner each evening.
//! \file    mlDinner.h
//! \author  Mister Misterman

//! ML module class to prepare a dinner each evening.
class Dinner : public BaseOp {

  //! Enumerator identifiers for dinner objects.
  enum DinnerObjects {
    FORK = 0,     //!< Id for fork objects.
    PLATE         //!< Id for plate objects.
  };

  //! Cooks the complete dinner.
  //! \param  numForks  is the number of forks to lay on the table.
  //! \param  numPlates is the number of plates to lay on the table.
  //! \return is the complete number of objects layed on the table.
  int createDinner(int numForks, int numPlates);

  //! Any C++ member documentation.
  float _anyMember;

  //! \name Function group creating dinner objects.
  //@{
  //! Creates and lays a fork on the table.
  void _createFork();

  //! Creates and lays a plate on the table.
  void createPlate();
  //@}

};
\endverbatim

*/
