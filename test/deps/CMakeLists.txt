#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(p2p_outcome_test outcome_test.cpp)
target_link_libraries(p2p_outcome_test
    Boost::boost
    )

addtest(p2p_di_test di_test.cpp)
target_link_libraries(p2p_di_test
    Boost::Boost.DI
    )
