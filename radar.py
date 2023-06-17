import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.cm import ScalarMappable

class radar:
    def __init__(self):
        # Create the figure and subplot
        fig = plt.figure(figsize=(8, 8))
        self.ax = fig.add_subplot(111, polar=True)

        # Set the title and grid
        self.ax.set_title("Radar Monitor", fontsize=20, fontweight='bold')
        self.ax.grid(True)

        # Set the radial tick labels
        self.ax.set_yticklabels([])

        # Set the azimuthal tick labels
        tick_positions = np.arange(0, 181, 10)
        self.ax.set_xticks(np.deg2rad(tick_positions))
        self.ax.set_xticklabels([str(i) for i in tick_positions])

        # Create a ScalarMappable object for the colorbar
        sm = ScalarMappable(cmap=plt.cm.gray)
        sm.set_array([])  # Set an empty array for now

        # Add a colorbar to indicate object intensity
        cbar = plt.colorbar(sm, ax=self.ax, orientation='horizontal')
        cbar.ax.set_xlabel('Object Intensity')

    def update_radar_display(self,object_data):
        # Generate random radar data for each frame

        # Clear the current plot
        self.ax.clear()

        # Set the theta values for the radar plot
        theta = np.deg2rad(np.arange(0, 180))

        # Plot the data points
        self.ax.plot(theta, object_data)

        # Find the indices where objects are detected
        object_indices = np.where(object_data > 0)[0]

        # Calculate the intensities of the detected objects
        intensities = object_data[object_indices]

        # Plot markers for the detected objects
        self.ax.scatter(np.deg2rad(object_indices), intensities, c=intensities, cmap=plt.cm.jet, alpha=0.5,linestyle='')

        # Fill the area inside the radar plot
        self.ax.fill(theta, object_data, alpha=0.25)

        # Set the title and grid
        self.ax.set_title("Radar Monitor", fontsize=20, fontweight='bold')
        self.ax.grid(True)

        # Set the radial tick labels
        self.ax.set_yticklabels([])

        # Set the azimuthal tick labels
        # self.ax.set_xticks(np.deg2rad(tick_positions))
        # self.ax.set_xticklabels([str(i) for i in tick_positions])

        # Update the colorbar limits with the data range
        # sm.set_clim(np.min(object_data), np.max(object_data))

# Create the animation
#ani = FuncAnimation(fig, update_radar_display, interval=1000)

r = radar()
while(1):
    object_data = np.random.randint(0, 100, size=180)

    r.update_radar_display(object_data)
    plt.pause(0.1)
    print("test")


# Show the radar monitor
plt.show()

