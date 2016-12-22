// Copyright 2010 Complete Genomics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You
// may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the License.

#include "cgatools/core.hpp"
#include "cgatools/util/Exception.hpp"
#include "cgatools/command/Fasta2Crr.hpp"
#include "cgatools/command/Crr2Fasta.hpp"
#include "cgatools/command/ListCrr.hpp"
#include "cgatools/command/DecodeCrr.hpp"
#include "cgatools/command/SnpDiff.hpp"
#include "cgatools/command/ListVariants.hpp"
#include "cgatools/command/TestVariants.hpp"
#include "cgatools/command/CallDiff.hpp"
#include "cgatools/command/Map2Sam.hpp"
#include "cgatools/command/MergedMap2Sam.hpp"
#include "cgatools/command/Evidence2Sam.hpp"
#include "cgatools/command/Evidence2Cache.hpp"
#include "cgatools/command/Sam2Reads.hpp"
#include "cgatools/command/Join.hpp"
#include "cgatools/command/JunctionDiff.hpp"
#include "cgatools/command/JunctionCmp.hpp"
#include "cgatools/command/JunctionType.hpp"
#include "cgatools/command/VarFileCombine.hpp"
#include "cgatools/command/VarFilter.hpp"
#include "cgatools/command/MkVcf.hpp"
#include "cgatools/command/Junction2Vcf.hpp"

#include <exception>
#include <iostream>
#include <sstream>

#include <boost/foreach.hpp>
#include <boost/format.hpp>

using namespace std;

using cgatools::util::Exception;
using cgatools::command::Command;
namespace po = boost::program_options;

class HelpCommand : public Command
{
public:
    HelpCommand(const std::string& name)
        : Command(name,
                  "Prints help information.",
                  "",
                  "")
    {
        options_.add_options()
            ("command", po::value<string>(&command_),
             "The command to describe.")
            ("format", po::value<string>(&format_)->default_value("text"),
             "The format of the output stream (text or html).")
            ("output", po::value<string>(&outputFileName_)->default_value("STDOUT"),
             "The output file (may be omitted for stdout).")
            ;

        positionalOptions_.add("command", 1);
    }

    static void listCommands(std::ostream& out, bool html, bool addLinks);
    static void writeHtmlHeader(std::ostream& out);
    static void writeHtmlFooter(std::ostream& out);

protected:
    int run(po::variables_map& vm);
        
private:
    std::string command_;
    std::string format_;
    std::string outputFileName_;
};

class ManCommand : public Command
{
public:
    ManCommand(const std::string& name)
        : Command(name,
                  "Prints the cgatools reference manual.",
                  "",
                  "")
    {
        options_.add_options()
            ("output", po::value<string>(&outputFileName_)->default_value("STDOUT"),
             "The output file (may be omitted for stdout).")
            ("format", po::value<string>(&format_)->default_value("text"),
             "The format of the output stream (text or html).")
            ;
    }

protected:
    int run(po::variables_map& vm);

private:
    std::string outputFileName_;
    std::string format_;
};

boost::shared_ptr<Command> getCommand(const char* name)
{
    boost::shared_ptr<Command> command;
    if (0 == strcmp("-h", name)) command.reset(new HelpCommand(name));
    if (0 == strcmp("--help", name)) command.reset(new HelpCommand(name));
#define CGA_COMMAND_MAP(cmdname, commandname) \
    if (0 == strcmp(name, cmdname)) command.reset(new commandname(cmdname));
#include "cgatoolsexe/cgatoolscommandmap.hpp"
    if (0 == command.get())
        throw Exception("unknown command name: "+string(name));
    return command;
}

void HelpCommand::listCommands(std::ostream& out, bool html, bool addLinks)
{
    const char* cmdlist[] =
    {
#undef CGA_COMMAND_MAP
#define CGA_COMMAND_MAP(cmdname, commandname) cmdname,
#include "cgatoolsexe/cgatoolscommandmap.hpp"
    };
    size_t maxLen = 0;
    BOOST_FOREACH(const char* cmdname, cmdlist)
    {
        maxLen = std::max(maxLen, strlen(cmdname));
    }

    out << "cgatools version " << CGA_TOOLS_VERSION << " build " << CGA_TOOLS_BUILD;
    if (CGA_TOOLS_DEVBUILD)
        out << " (dev)";
    out << endl;
    out << "usage: cgatools COMMAND [ options ] [ positionalArgs ]" << endl;
    out << "\nFor help on a particular command CMD, try \"cgatools help CMD\"." << endl;
    out << "Available commands:" << endl;
    string formatSpec = (boost::format("%%-%ds  \t%%s") % maxLen).str();
    ostringstream oss;
    BOOST_FOREACH(const char* cmdname, cmdlist)
    {
        if (0 == strcmp(cmdname, "map2sam"))
            continue;
        boost::shared_ptr<Command> command = getCommand(cmdname);
        string name = cmdname;
        if (html && addLinks)
            name = string("<a href=\"#") + cmdname + "\">" + cmdname + "</a>";
        for(size_t ii=0; ii<maxLen-strlen(cmdname); ii++)
            name.push_back(' ');
        oss << name << "  \t" << command->getShortDescription() << "\n";
    }
    out << Command::formatDescription(oss.str(), 4, Command::getHelpLineLength(), html);
}

void HelpCommand::writeHtmlHeader(std::ostream& out)
{
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" "
        "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
        "<head>\n"
        "<title>cgatools " << CGA_TOOLS_VERSION << " manual</title>\n"
        "<style type=\"text/css\">\n"
        "<!--\n"
        "pre {   font-family:\"courier new\", monospace;   font-size:9pt;}\n"
        "-->\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<pre>";
}

void HelpCommand::writeHtmlFooter(std::ostream& out)
{
    out << "</pre>\n"
        "</body>\n"
        "</html>\n";
}

int HelpCommand::run(po::variables_map& vm)
{
    if ("html" != format_ && "text" != format_)
        throw Exception("unrecognized format: "+format_);

    std::ostream& out = openStdout(outputFileName_);

    if (0 == vm.count("command") || "commands" == command_)
    {
        if ("html" == format_)
            writeHtmlHeader(out);
        listCommands(out, "html" == format_, false);
        if ("html" == format_)
            writeHtmlFooter(out);
        return 0;
    }

    if (vm.count("command") > 0)
    {
        if ("html" == format_)
            writeHtmlHeader(out);
        getCommand(command_.c_str())->help(out, "html" == format_);
        if ("html" == format_)
            writeHtmlFooter(out);
        return 0;
    }

    CGA_ASSERT(false);
    return 0;
}

int ManCommand::run(po::variables_map& vm)
{
    if ("html" != format_ && "text" != format_)
        throw Exception("unrecognized format: "+format_);

    std::ostream& out = openStdout(outputFileName_);

    if ("html" == format_)
        HelpCommand::writeHtmlHeader(out);
    HelpCommand::listCommands(out, "html" == format_, true);

    const char* cmdlist[] =
    {
#undef CGA_COMMAND_MAP
#define CGA_COMMAND_MAP(cmdname, commandname) cmdname,
#include "cgatoolsexe/cgatoolscommandmap.hpp"
    };

    BOOST_FOREACH(const char* cmdname, cmdlist)
    {
        if (0 == strcmp(cmdname, "map2sam"))
            continue;
        out << "\n\n-----------------------------------"
            "--------------------------------------------\n\n";
        getCommand(cmdname)->help(out, "html" == format_);
    }

    if ("html" == format_)
        HelpCommand::writeHtmlFooter(out);

    return 0;
}

int main(int argc, char* argv[])
{
    const char* DEFAULT_ARGV[] =
    {
        "cgatools",
        "help",
        "commands",
        0
    };
    try
    {
        if (argc < 2)
        {
            argc = sizeof(DEFAULT_ARGV)/sizeof(DEFAULT_ARGV[0]) - 1;
            argv = const_cast<char**>(DEFAULT_ARGV);
        }

        boost::shared_ptr<Command> command = getCommand(argv[1]);
        return (*command)(argc, argv);
    }
    catch(std::exception& ee)
    {
        cerr << argv[0] << " " << argv[1] << ": " << ee.what() << endl;
    }
    catch (...)
    {
        cerr << argv[0] << " " << argv[1] << ": An unknown exception occurred." << endl;
    }

    return 2;
}
