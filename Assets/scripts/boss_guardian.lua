-- EPITECH PROJECT, 2025
-- G-CPP-500-COT-5-1-rtype-24
-- File description:
-- boss_guardian.lua


function init(self)
    self.phase = 1
    self.attack_timer = 3.0
end

function on_update(self, dt)
    self.attack_timer = self.attack_timer - dt

    if self.attack_timer <= 0 then
        if self.phase == 1 then
            fire_circle_pattern(self, 12, 150)
            self.attack_timer = 4.0

        elseif self.phase == 2 then
            fire_aimed_shot(self, 250)
            self.attack_timer = 1.5
        end
    end
end

function on_damage(self, amount)
    if self:getHealth() < 500 and self.phase == 1 then
        print("GUARDIAN ENRAGED! ENTERING PHASE 2!")
        self.phase = 2
        
        local vel = self:getVelocity()
        self:setVelocity(vel.dx * 1.5, vel.dy)
    end
end

--- Fonctions utilitaires ---

-- NOTE: Pour que ces fonctions marchent, il faudra améliorer notre API C++.
-- Mais pour l'instant, le boss va apparaître et réagir aux dégâts.

function fire_circle_pattern(self, bullet_count, speed)
    local pos = self:getPosition()
    local angle_step = 360 / bullet_count

    for i = 0, bullet_count - 1 do
        local angle_rad = math.rad(i * angle_step)
        local dx = math.cos(angle_rad) * speed
        local dy = math.sin(angle_rad) * speed
        
        -- Cette ligne va spawner la balle, mais elle ira tout droit pour l'instant.
        -- C'est normal, on n'a pas encore la fonction pour changer sa vélocité.
        engine.spawn_from_archetype("projectiles.enemy_bullet_slow", pos.x, pos.y)
    end
end

function fire_aimed_shot(self, speed)
    local boss_pos = self:getPosition()
    local player_pos = engine.get_player_position()

    if player_pos then
        local dir_x = player_pos.x - boss_pos.x
        local dir_y = player_pos.y - boss_pos.y
        local magnitude = math.sqrt(dir_x^2 + dir_y^2)

        local dx = (dir_x / magnitude) * speed
        local dy = (dir_y / magnitude) * speed

        engine.spawn_from_archetype("projectiles.enemy_bullet_slow", boss_pos.x, boss_pos.y)
    end
end