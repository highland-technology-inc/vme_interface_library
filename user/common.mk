# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Author: Dean W. Anneser
# Company: RTLinux Solutions LLC for Highland Technology, Inc.
# Date: Sat Aug 07 16:50:58 2021

# common targets

%.o: %.c $(HEADERS)
	umask 002; $(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%: %.o
	umask 002; $(CC) $< -o $@ $(LFLAGS) $(LIBS) 

.PHONY: clean
clean:
	@rm -f $(ALL) $(OBJECTS) *~ \#*

.PHONY: printvars
printvars:
	@$(foreach V,$(sort $(.VARIABLES)), $(if $(filter-out environment% default automatic, $(origin $V)),$(warning $V=$($V) ($(value $V)))))
