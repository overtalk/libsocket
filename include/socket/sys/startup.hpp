/**
 * Created by Jian Chen
 * @since  2016.09.08
 * @author Jian Chen <admin@chensoft.com>
 * @link   http://chensoft.com
 */
#pragma once

namespace chen
{
    namespace bsd
    {
        /**
         * Global initialization
         */
        class startup
        {
        public:
            startup();
        };

        static const startup setup_dummy;  // force linking the setup code
    }
}