/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <map>
#include <string>
#include <vector>

#include <cm/optional>

#include "cmArgumentParser.h"
#include "cmArgumentParserTypes.h"
#include "cmStateTypes.h"

class cmMakefile;
template <typename Iter>
class cmRange;

/** \class cmCoreTryCompile
 * \brief Base class for cmTryCompileCommand and cmTryRunCommand
 *
 * cmCoreTryCompile implements the functionality to build a program.
 * It is the base class for cmTryCompileCommand and cmTryRunCommand.
 */
class cmCoreTryCompile
{
public:
  cmCoreTryCompile(cmMakefile* mf)
    : Makefile(mf)
  {
  }

  struct Arguments : public ArgumentParser::ParseResult
  {
    cm::optional<std::string> CompileResultVariable;
    cm::optional<std::string> BinaryDirectory;
    cm::optional<std::string> SourceDirectoryOrFile;
    cm::optional<std::string> ProjectName;
    cm::optional<std::string> TargetName;
    cm::optional<ArgumentParser::NonEmpty<std::vector<std::string>>> Sources;
    ArgumentParser::MaybeEmpty<std::vector<std::string>> CMakeFlags{
      1, "CMAKE_FLAGS"
    }; // fake argv[0]
    std::vector<std::string> CompileDefs;
    cm::optional<ArgumentParser::MaybeEmpty<std::vector<std::string>>>
      LinkLibraries;
    ArgumentParser::MaybeEmpty<std::vector<std::string>> LinkOptions;
    std::map<std::string, std::string> LangProps;
    std::string CMakeInternal;
    cm::optional<std::string> OutputVariable;
    cm::optional<std::string> CopyFileTo;
    cm::optional<std::string> CopyFileError;

    // Argument for try_run only.
    // Keep in sync with warnings in cmCoreTryCompile::ParseArgs.
    cm::optional<std::string> CompileOutputVariable;
    cm::optional<std::string> RunOutputVariable;
    cm::optional<std::string> RunOutputStdOutVariable;
    cm::optional<std::string> RunOutputStdErrVariable;
    cm::optional<std::string> RunWorkingDirectory;
    cm::optional<ArgumentParser::MaybeEmpty<std::vector<std::string>>> RunArgs;
  };

  Arguments ParseArgs(cmRange<std::vector<std::string>::const_iterator> args,
                      bool isTryRun);

  /**
   * This is the core code for try compile. It is here so that other commands,
   * such as TryRun can access the same logic without duplication.
   *
   * This function requires at least two \p arguments and will crash if given
   * fewer.
   */
  bool TryCompileCode(Arguments& arguments,
                      cmStateEnums::TargetType targetType);

  /**
   * Returns \c true if \p path resides within a CMake temporary directory,
   * otherwise returns \c false.
   */
  static bool IsTemporary(std::string const& path);

  /**
   * This deletes all the files created by TryCompileCode.
   * This way we do not have to rely on the timing and
   * dependencies of makefiles.
   */
  void CleanupFiles(std::string const& binDir);

  /**
   * This tries to find the (executable) file created by
  TryCompileCode. The result is stored in OutputFile. If nothing is found,
  the error message is stored in FindErrorMessage.
   */
  void FindOutputFile(const std::string& targetName);

  std::string BinaryDirectory;
  std::string OutputFile;
  std::string FindErrorMessage;
  bool SrcFileSignature = false;
  cmMakefile* Makefile;

private:
  Arguments ParseArgs(
    const cmRange<std::vector<std::string>::const_iterator>& args,
    const cmArgumentParser<Arguments>& parser,
    std::vector<std::string>& unparsedArguments);
};
