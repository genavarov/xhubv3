//******************************************************************************
//******************************************************************************

#include "settings.h"
#include "logger.h"
#include "../config.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

//******************************************************************************
//******************************************************************************
Settings & settings()
{
    static Settings s;
    return s;
}

//******************************************************************************
//******************************************************************************
Settings::Settings()
    : m_isExchangeEnabled(false)
    , m_dhtPort(Config::DHT_PORT)
{
}

//******************************************************************************
//******************************************************************************
bool Settings::parseCmdLine(int argc, char * argv[])
{
    m_appPath = std::string(argv[0]);

    boost::program_options::options_description description("allowed options");
    description.add_options()
            ("dhtport",    boost::program_options::value<unsigned short>(), "dht listen port")
            ("bridgeport", boost::program_options::value<unsigned short>(), "xbridge listen port")
            ("peer",       boost::program_options::value<std::vector<std::string> >(), "connect to peer")
            ("enable-exchange", "enable exchange service");

    boost::program_options::variables_map options;
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), options);
    }
    catch (const std::exception& e)
    {
        LOG() << "command line error: " << e.what();
        return false;
    }

    if (options.count("enable-exchange"))
    {
        m_isExchangeEnabled = true;
    }
    if (options.count("dhtport"))
    {
        m_dhtPort = options["dhtport"].as<unsigned short>();
    }
    if (options.count("peer"))
    {
        m_peers = options["peer"].as<std::vector<std::string> >();
    }

    return true;
}

//******************************************************************************
//******************************************************************************
bool Settings::read(const char * fileName)
{
    try
    {
        if (fileName)
        {
            m_fileName = std::string(fileName);
        }

        if (m_fileName.empty())
        {
            return false;
        }

        boost::property_tree::ini_parser::read_ini(m_fileName, m_pt);
    }
    catch (std::exception & e)
    {
        LOG() << e.what();
        return false;
    }

    return true;
}

//******************************************************************************
//******************************************************************************
bool Settings::write(const char * fileName)
{
    try
    {
        std::string iniName = m_fileName;
        if (fileName)
        {
            iniName = std::string(fileName);
        }

        if (iniName.empty())
        {
            return false;
        }

        boost::property_tree::ini_parser::write_ini(iniName, m_pt);
    }
    catch (std::exception & e)
    {
        LOG() << e.what();
        return false;
    }

    return true;
}

//******************************************************************************
//******************************************************************************
std::vector<std::string> Settings::exchangeWallets() const
{
    std::string list;
    TRY(list = m_pt.get<std::string>("Main.ExchangeWallets"));

    std::vector<std::string> strs;
    if (list.size() > 0)
    {
        boost::split(strs, list, boost::is_any_of(",;:"));
    }

    return strs;
}
