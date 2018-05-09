#include "logger.hpp"

logging::logger< logging::file_log_policy > log_inst( "execution.log" );
//logging::logger< logging::stdout_log_policy > log_inst( "execution.log" );
