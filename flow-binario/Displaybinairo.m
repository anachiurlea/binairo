file = "Binairo_6x6.png";
feasible = true;

original = [
    NaN, 0 , NaN, NaN, NaN, NaN, ;
    NaN, NaN, 0 , NaN, 0 , NaN, ;
    NaN, NaN, NaN, NaN, 1 , 0 , ;
    1 , 1 , NaN, NaN, NaN, NaN, ;
    NaN, 0 , NaN, 0 , NaN, NaN, ;
    NaN, NaN, NaN, NaN, 0 , NaN, ;
    ];

solution = [
    1, 0, 0, 1, 1, 0, ;
    0, 1, 0, 1, 0, 1, ;
    1, 0, 1, 0, 1, 0, ;
    1, 1, 0, 1, 0, 0, ;
    0, 0, 1, 0, 1, 1, ;
    0, 1, 1, 0, 0, 1, ;
    ];

function displayBinairo(Original, Solution, file, feasible)
    N = size(Original,1);
    % create a new figure
    figure;

    % keep the figure up for the drawings
    hold on;

    % configure axes
    axis equal;
    axis off;
    axis([-0.5 6.5 -0.5 7.5]);

    % fixed background size
    rectangle('Position', [-0.5 -0.5 7.5 7.5], 'FaceColor', 'w', 'EdgeColor', 'k');

    % display grid to adapt to the standard size of a grid
    % We want the grid to occupy the original 0 to 6 bounds
    grid_min = 0;
    grid_max = 6;

    % linspace handles the math so N elements always span exactly from grid_min to grid_max
    t = linspace(grid_min, grid_max, N + 1); 
    [X, Y] = meshgrid(t, t);
    plot(X, Y, 'k');
    plot(X', Y', 'k');
    


    % display numbers
    for i = 1:6
        for j = 1:6
            color = 'k';
            if isnan(Original(i,j))
                color = 'b';
            end
            text(j - 0.5, 6.5 - i, num2str(Solution(i, j)),'Color', color, 'FontWeight', 'bold', 'HorizontalAlignment', 'center', 'VerticalAlignment', 'middle', 'FontSize', 24);
        end
    end
   
    % display name and date
    time = datetime('now');
    text(3.5, 6.5, string(time), 'Color', 'k', 'FontSize', 18);
    text(0, 6.5, string(file),'Interpreter', 'none', 'Color', 'b', 'FontWeight', 'bold', 'FontSize', 18)
    hold off; 
end

displayBinairo(original, solution, file, feasible)