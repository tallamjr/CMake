/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "cmIfCommand.h"
#include "cmCacheManager.h"

bool cmIfFunctionBlocker::
IsFunctionBlocked(const char *name, const std::vector<std::string> &args, 
                  const cmMakefile &mf) const
{
  if (!strcmp(name,"ELSE") || !strcmp(name,"ENDIF"))
    {
    if (m_Not && (args.size() == 2) && !strcmp(args[0].c_str(),"NOT") &&
        !strcmp(args[1].c_str(),m_Define.c_str()))
      {
      return false;
      }
    if (!m_Not && (args.size() == 1) && 
        !strcmp(args[0].c_str(),m_Define.c_str()))
      {
      return false;
      }
    }
  return true;
}

bool cmIfFunctionBlocker::
ShouldRemove(const char *name, const std::vector<std::string> &args, 
             const cmMakefile &mf) const
{
  return !this->IsFunctionBlocked(name,args,mf);
}

void cmIfFunctionBlocker::
ScopeEnded(const cmMakefile &mf) const
{
  cmSystemTools::Error("The end of a CMakeLists file was reached with an IF statement that was not closed properly. Within the directory: ", 
                       mf.GetCurrentDirectory(),
                       (m_Not ? " The arguments to the if were: NOT " : " The arguments to the if were: "),
                       m_Define.c_str());
}

bool cmIfCommand::InitialPass(std::vector<std::string>& args)
{
  if(args.size() < 1 )
    {
    this->SetError("called with incorrect number of arguments");
    return false;
    }

  // check for the NOT value
  const char *def;
  if (args.size() == 2 && (args[0] == "NOT"))
    {
    def = m_Makefile->GetDefinition(args[1].c_str());
    if(!cmSystemTools::IsOff(def))
      {
      // create a function blocker
      cmIfFunctionBlocker *f = new cmIfFunctionBlocker();
      f->m_Define = args[1];
      f->m_Not = true;
      m_Makefile->AddFunctionBlocker(f);
      }
    else
      {
      // do nothing
      }
    }
  else
    {
    def = m_Makefile->GetDefinition(args[0].c_str());
    if(!cmSystemTools::IsOff(def))
      {
      // do nothing
      }
    else
      {
      // create a function blocker
      cmIfFunctionBlocker *f = new cmIfFunctionBlocker();
      f->m_Define = args[0];
      m_Makefile->AddFunctionBlocker(f);
      }
    }
  
  return true;
}

