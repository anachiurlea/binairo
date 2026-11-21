function [grid, valid] = solvebinairo(grid)
n = size(grid, 1);

% 1. Filling in all the direct cells first
[grid, ok] = DirectValues(grid, n);
if ~ok
    valid = false;
    return; % Dead end, one of the rules was broken
end

% 2. Check if puzzle is already fully solved with the direct values step
if ~any(isnan(grid(:)))
    valid = true;
    return; % No NaNs left means it's solved
end

% 3. Find the first empty (NaN) cell to guess on
[r, c] = find(isnan(grid), 1);

% 4. Try guessing 0 first.
grid0 = grid;
grid0(r, c) = 0;
[sol0, valid0] = SolveBinairo(grid0);
if valid0
    grid = sol0;
    valid = true;
    return;
end

% 5. If 0 failed, try guessing 1
grid1 = grid;
grid1(r, c) = 1;
[sol1, valid1] = SolveBinairo(grid1);
if valid1
    grid = sol1;
    valid = true;
    return;
end

% If both 0 and 1 failed
valid = false;
end

function [grid, ok] = DirectValues(grid, n)
ok = true;
changed = true;
while changed  % when the grid is changed, it means we can continue, but 
    % if there is no change, there are no more direct inputs left to make 
    %without guessing, so we exit the loop.
    changed = false;
    for j = 1:n
        for k = 1:n
            if isnan(grid(j, k))
                ok0 = CheckValidMove(grid, j, k, 0, n);
                ok1 = CheckValidMove(grid, j, k, 1, n);

                if ~ok0 && ~ok1
                    ok = false;
                    return;
                elseif ok0 && ~ok1
                    grid(j, k) = 0;
                    changed = true;
                elseif ~ok0 && ok1
                    grid(j, k) = 1;
                    changed = true;
                end
            end
        end
    end
end
end

function ok = CheckValidMove(grid, r, c, v, n) %check if placing value v at
%position (r,c) is valid
grid(r, c) = v;
ok = CheckVectorOk(grid(r, :), n) && ...
    CheckVectorOk(grid(:, c), n) && ...
    CheckVectorUniqueOk(grid, r, c, n);
end

function ok = CheckVectorOk(vector, n)
ok = true;
for idx = 1:n-2
    if vector(idx) == vector(idx+1) && vector(idx+1) == vector(idx+2)
        ok = false;
        break;
    end
end
if length(find(vector == 0)) > n/2 || length(find(vector == 1)) > n/2
    ok = false;
end
end

function ok = CheckVectorUniqueOk(grid, r, c, n)
ok = true;

% Check row r if it has no NaNs
if ~any(isnan(grid(r, :)))
    for idx = 1:n
        % Validate uniqueness for the specific row and column.
        if idx ~= r && isequal(grid(r, :), grid(idx, :))
            ok = false;
            return;
        end
    end
end

% Check col c if it has no NaNs
if ~any(isnan(grid(:, c)))
    for idx = 1:n
        if idx ~= c && isequal(grid(:, c), grid(:, idx))
            ok = false;
            return;
        end
    end
end
end

function printGrid(grid)
disp(grid);
end
